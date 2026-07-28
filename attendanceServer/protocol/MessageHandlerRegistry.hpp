#pragma once

#include <functional>
#include <string>
#include <unordered_map>

namespace protocol {

// 消息处理器注册表（模板版本）
// Session, Envelope, Context 由使用者指定
template<typename Session, typename Envelope, typename Context>
class MessageHandlerRegistryT {
 public:
  using MessageHandler = std::function<void(
      Session& session,
      const Envelope& env,
      Context& ctx,
      const std::string& route_to)>;

  // 注册消息处理器
  void register_handler(const std::string& type, MessageHandler handler) {
    handlers_[type] = std::move(handler);
  }

  // 查找消息处理器，返回 nullptr 表示未注册
  const MessageHandler* find(const std::string& type) const {
    auto it = handlers_.find(type);
    return it != handlers_.end() ? &it->second : nullptr;
  }

  // 获取已注册的消息类型数量
  std::size_t size() const { return handlers_.size(); }

 private:
  std::unordered_map<std::string, MessageHandler> handlers_;
};

}  // namespace protocol