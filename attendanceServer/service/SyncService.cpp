#include "service/SyncService.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>

#include <mysqlx/xdevapi.h>
#include <nlohmann/json.hpp>

#include "db/FaceDataRepository.hpp"
#include "db/PersonRepository.hpp"
#include "net/Framing.hpp"
#include "net/Session.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"

namespace service {

namespace {

constexpr int kPersonPageLimit = 200;
constexpr int kFacePageLimit = 100;
constexpr int kMaxSyncFailures = 3;
constexpr std::size_t kJsonLineBudget =
    net::kMaxJsonLineBytes > 8192 ? net::kMaxJsonLineBytes - 8192 : 1;

std::int64_t utc_ms_now() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}

std::string json_string_any_of(const nlohmann::json& data,
                               std::initializer_list<const char*> keys) {
  for (const char* k : keys) {
    if (data.contains(k) && data[k].is_string()) {
      const auto s = data[k].get<std::string>();
      if (!s.empty()) {
        return s;
      }
    }
  }
  return {};
}

nlohmann::json person_to_json(const db::PersonRecord& p) {
  return {{"id", p.id},
          {"name", p.name},
          {"employeeId", p.employee_id},
          {"department", p.department},
          {"position", p.position}};
}

std::string build_person_sync_line(const std::string& device_id,
                                   const std::string& in_reply_to,
                                   const nlohmann::json& persons_array) {
  const std::int64_t ts = utc_ms_now();
  nlohmann::json j = {
      {"type", std::string(protocol::kTypePersonSync)},
      {"role", std::string(protocol::kRoleServer)},
      {"from", std::string(protocol::kRoleServer)},
      {"to", device_id},
      {"inReplyTo", in_reply_to},
      {"ts", ts},
      {"code", 0},
      {"msg", "ok"},
      {"deviceId", device_id},
      {"data",
       {{"deviceId", device_id}, {"persons", persons_array}}}};
  return j.dump();
}

void send_person_page_batches(const std::shared_ptr<net::ISession>& session,
                              const std::string& device_id,
                              const std::string& in_reply_to,
                              const std::vector<db::PersonRecord>& page) {
  nlohmann::json batch = nlohmann::json::array();

  for (const auto& p : page) {
    nlohmann::json trial = batch;
    trial.push_back(person_to_json(p));
    std::string line = build_person_sync_line(device_id, in_reply_to, trial);
    if (line.size() > kJsonLineBudget) {
      if (batch.empty()) {
        std::cerr << "sync: single person_sync row exceeds json line budget\n";
        session->write_line(protocol::build_error(
            protocol::kCodeBusinessValidation, "person row too large for line",
            in_reply_to, device_id));
        throw std::runtime_error("person_sync line budget");
      }
      session->write_line(
          build_person_sync_line(device_id, in_reply_to, batch));
      batch = nlohmann::json::array();
      trial = batch;
      trial.push_back(person_to_json(p));
      line = build_person_sync_line(device_id, in_reply_to, trial);
      if (line.size() > kJsonLineBudget) {
        session->write_line(protocol::build_error(
            protocol::kCodeBusinessValidation, "person row too large for line",
            in_reply_to, device_id));
        throw std::runtime_error("person_sync line budget");
      }
    }
    batch = std::move(trial);
  }

  if (!batch.empty()) {
    session->write_line(
        build_person_sync_line(device_id, in_reply_to, batch));
  }
}

std::string build_face_phase_line(std::string_view sync_type,
                                  const std::string& device_id,
                                  const std::string& in_reply_to) {
  const std::int64_t ts = utc_ms_now();
  nlohmann::json j = {
      {"type", std::string(sync_type)},
      {"role", std::string(protocol::kRoleServer)},
      {"from", std::string(protocol::kRoleServer)},
      {"to", device_id},
      {"inReplyTo", in_reply_to},
      {"ts", ts},
      {"code", 0},
      {"msg", "ok"},
      {"deviceId", device_id},
      {"data", {{"deviceId", device_id}}}};
  return j.dump();
}

std::string build_face_item_header_line(
    const std::string& device_id, const std::string& in_reply_to,
    const std::string& employee_id, std::uint64_t payload_len) {
  const std::int64_t ts = utc_ms_now();
  nlohmann::json j = {
      {"type", std::string(protocol::kTypeFaceSyncItemHeader)},
      {"role", std::string(protocol::kRoleServer)},
      {"from", std::string(protocol::kRoleServer)},
      {"to", device_id},
      {"inReplyTo", in_reply_to},
      {"ts", ts},
      {"code", 0},
      {"msg", "ok"},
      {"deviceId", device_id},
      {"data",
       {{"deviceId", device_id},
        {"employeeId", employee_id},
        {"featureSize", payload_len},
        {"payloadLength", payload_len},
        {"contentType", "application/octet-stream"},
        {"payloadEncoding", "raw"}}}};
  return j.dump();
}

void finish_sync_push(const std::shared_ptr<protocol::SyncPushGate>& gate) {
  if (!gate) {
    return;
  }
  std::lock_guard<std::mutex> lk(gate->mu);
  gate->push_running = false;
}

}  // namespace

void sync_handle_request(const protocol::ParsedEnvelope& env,
                         protocol::ConnContext& ctx, const std::string& reply_to,
                         DbExecutor& db,
                         const std::shared_ptr<net::ISession>& session) {
  if (ctx.endpoint_role != protocol::EndpointRole::Device) {
    session->write_line(protocol::build_error(
        protocol::kCodeForbidden, "device role required", env.msg_id,
        reply_to));
    return;
  }

  const std::string device_id = ctx.device_id;
  if (device_id.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing bound device_id",
        env.msg_id, reply_to));
    return;
  }

  const std::string data_device =
      json_string_any_of(env.data, {"deviceId", "device_id"});
  if (!data_device.empty() && data_device != device_id) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "deviceId mismatch", env.msg_id,
        reply_to));
    return;
  }

  const std::shared_ptr<protocol::SyncPushGate> gate = ctx.sync_gate;
  {
    std::lock_guard<std::mutex> lk(gate->mu);
    if (gate->push_running) {
      session->write_line(protocol::build_error(
          protocol::kCodeBusinessValidation, "sync already in progress",
          env.msg_id, reply_to));
      return;
    }
    if (gate->fail_count >= kMaxSyncFailures) {
      session->write_line(protocol::build_error(
          protocol::kCodeBusinessValidation,
          "sync temporarily blocked after repeated failures", env.msg_id,
          reply_to));
      return;
    }
    gate->push_running = true;
  }

  const std::string msg_id = env.msg_id;
  const std::weak_ptr<net::ISession> wp = session;

  auto run_face_pages =
      std::make_shared<std::function<void(const std::string&)>>();
  *run_face_pages = [=, &db](const std::string& after_employee_id) {
    auto rows = std::make_shared<std::vector<db::FaceDataRecord>>();
    db.dispatch(
        [after_employee_id, rows](mysqlx::Session& s) {
          db::FaceDataPageQuery q;
          q.after_employee_id = after_employee_id;
          q.limit = kFacePageLimit;
          *rows = db::FaceDataRepository::listFaceDataPage(s, q);
        },
        [=](std::exception_ptr ep) {
          auto sp = wp.lock();
          if (!sp) {
            finish_sync_push(gate);
            return;
          }
          if (ep) {
            finish_sync_push(gate);
            try {
              std::rethrow_exception(ep);
            } catch (const std::exception& ex) {
              std::cerr << "sync: face page db error: " << ex.what() << '\n';
              sp->write_line(protocol::build_error(
                  protocol::kCodeDbError, ex.what(), msg_id, device_id));
            }
            return;
          }

          for (const auto& rec : *rows) {
            const std::size_t plen = rec.feature_vector.size();
            if (plen == 0 ||
                static_cast<int>(plen) != rec.feature_size) {
              std::cerr << "sync: skip face row employeeId=" << rec.employee_id
                        << " bad feature size\n";
              continue;
            }
            if (plen > net::kMaxBinaryPayloadBytes) {
              finish_sync_push(gate);
              std::cerr << "sync: feature too large employeeId="
                        << rec.employee_id << '\n';
              sp->write_line(protocol::build_error(
                  protocol::kCodePayloadTooLarge, "face feature too large",
                  msg_id, device_id));
              return;
            }
            sp->write_line(build_face_item_header_line(
                device_id, msg_id, rec.employee_id,
                static_cast<std::uint64_t>(plen)));
            sp->write_binary(std::vector<char>(rec.feature_vector.begin(),
                                                 rec.feature_vector.end()));
          }

          if (rows->empty() ||
              static_cast<int>(rows->size()) < kFacePageLimit) {
            sp->write_line(build_face_phase_line(protocol::kTypeFaceSyncEnd,
                                                 device_id, msg_id));
            finish_sync_push(gate);
            return;
          }

          const std::string next_after = rows->back().employee_id;
          (*run_face_pages)(next_after);
        });
  };

  auto run_person_pages = std::make_shared<std::function<void(int)>>();
  *run_person_pages = [=, &db](int after_id) {
    auto page = std::make_shared<std::vector<db::PersonRecord>>();
    db.dispatch(
        [after_id, page](mysqlx::Session& s) {
          db::PersonPageQuery q;
          q.after_id = after_id;
          q.limit = kPersonPageLimit;
          *page = db::PersonRepository::listPersonPage(s, q);
        },
        [=](std::exception_ptr ep) {
          auto sp = wp.lock();
          if (!sp) {
            finish_sync_push(gate);
            return;
          }
          if (ep) {
            finish_sync_push(gate);
            try {
              std::rethrow_exception(ep);
            } catch (const std::exception& ex) {
              std::cerr << "sync: person page db error: " << ex.what() << '\n';
              sp->write_line(protocol::build_error(
                  protocol::kCodeDbError, ex.what(), msg_id, device_id));
            }
            return;
          }

          try {
            send_person_page_batches(sp, device_id, msg_id, *page);
          } catch (...) {
            finish_sync_push(gate);
            return;
          }

          if (page->empty()) {
            sp->write_line(build_face_phase_line(protocol::kTypeFaceSyncBegin,
                                                 device_id, msg_id));
            (*run_face_pages)(std::string{});
            return;
          }

          const int next_after = page->back().id;
          (*run_person_pages)(next_after);
        });
  };

  (*run_person_pages)(0);
}

void sync_handle_ack(const protocol::ParsedEnvelope& env,
                     protocol::ConnContext& ctx, const std::string& reply_to,
                     const std::shared_ptr<net::ISession>& session) {
  if (ctx.endpoint_role != protocol::EndpointRole::Device) {
    session->write_line(protocol::build_error(
        protocol::kCodeForbidden, "device role required", env.msg_id,
        reply_to));
    return;
  }

  const std::string status = json_string_any_of(env.data, {"status"});
  const std::string message = json_string_any_of(env.data, {"message"});

  const std::shared_ptr<protocol::SyncPushGate> gate = ctx.sync_gate;
  int displayed_fail = 0;
  {
    std::lock_guard<std::mutex> lk(gate->mu);
    if (status == "ok") {
      gate->fail_count = 0;
    } else {
      ++gate->fail_count;
    }
    displayed_fail = gate->fail_count;
  }

  std::cerr << "sync_ack deviceId=" << ctx.device_id
            << " msgId=" << env.msg_id << " inReplyTo=" << env.in_reply_to
            << " status=\"" << status << "\" message=\"" << message << "\""
            << " sync_fail_count=" << displayed_fail << '\n';
}

}  // namespace service
