#include "service/MessageRouter.hpp"

#include <chrono>

#include <nlohmann/json.hpp>

#include "net/Session.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"

namespace service {

namespace {

std::int64_t utc_ms() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}

std::string command_target_device(const protocol::ParsedEnvelope& env) {
  if (!env.to.empty()) {
    return env.to;
  }
  if (env.data.contains("to") && env.data["to"].is_string()) {
    return env.data["to"].get<std::string>();
  }
  if (env.data.contains("deviceId") && env.data["deviceId"].is_string()) {
    return env.data["deviceId"].get<std::string>();
  }
  return {};
}

}  // namespace

MessageRouter::MessageRouter(boost::asio::io_context& ioc,
                           IDeviceRegistry& devices)
    : ioc_(ioc), devices_(devices) {}

MessageRouter::~MessageRouter() {
  std::lock_guard<std::mutex> lk(mu_);
  for (auto& e : pending_) {
    if (e.second->timer) {
      e.second->timer->cancel();
    }
  }
  pending_.clear();
}

void MessageRouter::cancel_pending_for_session(const net::ISession* raw) {
  if (!raw) {
    return;
  }
  std::lock_guard<std::mutex> lk(mu_);
  for (auto it = pending_.begin(); it != pending_.end();) {
    auto sp = it->second->admin.lock();
    if (it->second->has_admin && (!sp || sp.get() == raw)) {
      if (it->second->timer) {
        it->second->timer->cancel();
      }
      it = pending_.erase(it);
    } else {
      ++it;
    }
  }
}

std::string MessageRouter::make_forward_id(const std::string& admin_msg_id) {
  return admin_msg_id + ".fw." + std::to_string(++seq_);
}

void MessageRouter::forward_device_command(
    const std::shared_ptr<net::ISession>& admin_sess,
    const protocol::ParsedEnvelope& env, const std::string& operator_id) {
  if (!admin_sess) {
    return;
  }
  const std::string device_id = command_target_device(env);
  if (device_id.empty()) {
    admin_sess->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing target device id",
        env.msg_id, operator_id));
    return;
  }

  const std::string forward_id = make_forward_id(env.msg_id);
  nlohmann::json root;
  root["type"] = std::string(protocol::kTypeDeviceCommand);
  root["role"] = std::string(protocol::kRoleServer);
  root["from"] = std::string(protocol::kRoleServer);
  root["to"] = device_id;
  root["msgId"] = forward_id;
  root["ts"] = env.ts != 0 ? env.ts : utc_ms();
  nlohmann::json data = env.data;
  data["originAdmin"] = operator_id;
  root["data"] = std::move(data);

  const std::string line = root.dump();

  bool delivered = false;
  devices_.with_session(device_id, [&](net::ISession& dev) {
    delivered = true;
    dev.write_line(line);
  });

  if (!delivered) {
    admin_sess->write_line(protocol::build_error(
        protocol::kCodeDeviceOffline, "device offline", env.msg_id,
        operator_id));
    return;
  }

  auto pend = std::make_unique<Pending>();
  pend->admin = admin_sess;
  pend->has_admin = true;
  pend->admin_msg_id = env.msg_id;
  pend->operator_id = operator_id;
  pend->timer = std::make_unique<boost::asio::steady_timer>(ioc_);

  const std::string fid = forward_id;

  // 先设置 timer，再持锁插入 pending_，避免插入后释放锁导致其他线程
  // 通过 handle_device_command_ack 销毁 Pending 而 raw_pending 悬空。
  pend->timer->expires_after(std::chrono::seconds(10));
  pend->timer->async_wait(
      [this, fid](const boost::system::error_code& ec) {
        if (ec == boost::asio::error::operation_aborted) {
          return;
        }
        std::unique_ptr<Pending> kept;
        {
          std::lock_guard<std::mutex> lk(mu_);
          auto it = pending_.find(fid);
          if (it == pending_.end()) {
            return;
          }
          kept = std::move(it->second);
          pending_.erase(it);
        }
        auto sp = kept->admin.lock();
        if (!sp) {
          return;
        }
        sp->write_line(protocol::build_error(
            protocol::kCodeForwardTimeout, "device.command timeout",
            kept->admin_msg_id, kept->operator_id));
      });
  {
    std::lock_guard<std::mutex> lk(mu_);
    pending_[fid] = std::move(pend);
  }
}

MessageRouter::ForwardResult MessageRouter::forward_server_device_command(
    const std::string& device_id, const nlohmann::json& data,
    const std::string& msg_id_seed, AckHandler on_ack,
    TimeoutHandler on_timeout) {
  ForwardResult result;
  if (device_id.empty()) {
    return result;
  }

  result.forward_id = make_forward_id(msg_id_seed.empty() ? "server" : msg_id_seed);

  nlohmann::json root;
  root["type"] = std::string(protocol::kTypeDeviceCommand);
  root["role"] = std::string(protocol::kRoleServer);
  root["from"] = std::string(protocol::kRoleServer);
  root["to"] = device_id;
  root["msgId"] = result.forward_id;
  root["ts"] = utc_ms();
  root["data"] = data;

  const std::string line = root.dump();
  devices_.with_session(device_id, [&](net::ISession& dev) {
    result.delivered = true;
    dev.write_line(line);
  });

  if (!result.delivered) {
    return result;
  }

  auto pend = std::make_unique<Pending>();
  pend->admin_msg_id = result.forward_id;
  pend->operator_id = std::string(protocol::kRoleServer);
  pend->on_ack = std::move(on_ack);
  pend->on_timeout = std::move(on_timeout);
  pend->timer = std::make_unique<boost::asio::steady_timer>(ioc_);

  const std::string fid = result.forward_id;

  // 先设置 timer，再持锁插入 pending_，避免 raw_pending 悬空 UAF。
  pend->timer->expires_after(std::chrono::seconds(10));
  pend->timer->async_wait(
      [this, fid](const boost::system::error_code& ec) {
        if (ec == boost::asio::error::operation_aborted) {
          return;
        }
        std::unique_ptr<Pending> kept;
        {
          std::lock_guard<std::mutex> lk(mu_);
          auto it = pending_.find(fid);
          if (it == pending_.end()) {
            return;
          }
          kept = std::move(it->second);
          pending_.erase(it);
        }
        if (kept->on_timeout) {
          kept->on_timeout(fid);
        }
      });
  {
    std::lock_guard<std::mutex> lk(mu_);
    pending_[fid] = std::move(pend);
  }

  return result;
}

void MessageRouter::handle_device_command_ack(
    const protocol::ParsedEnvelope& env) {
  const std::string& fwd = env.in_reply_to;
  if (fwd.empty()) {
    return;
  }

  std::unique_ptr<Pending> kept;
  {
    std::lock_guard<std::mutex> lk(mu_);
    auto it = pending_.find(fwd);
    if (it == pending_.end()) {
      return;
    }
    if (it->second->timer) {
      it->second->timer->cancel();
    }
    kept = std::move(it->second);
    pending_.erase(it);
  }

  if (kept->on_ack) {
    kept->on_ack(fwd, env);
  }

  auto admin_sp = kept->admin.lock();
  if (!admin_sp) {
    return;
  }

  nlohmann::json root;
  root["type"] = std::string(protocol::kTypeDeviceCommandAck);
  root["role"] = std::string(protocol::kRoleServer);
  root["from"] = std::string(protocol::kRoleServer);
  root["to"] = kept->operator_id;
  root["msgId"] = env.msg_id.empty() ? fwd : env.msg_id;
  root["inReplyTo"] = kept->admin_msg_id;
  root["ts"] = env.ts != 0 ? env.ts : utc_ms();
  root["code"] = env.code;
  root["msg"] = env.msg;
  root["data"] = env.data;

  admin_sp->write_line(root.dump());
}

}  // namespace service
