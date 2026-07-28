#include "util/SystemError.hpp"

#include <sstream>
#include <string>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace util {

namespace {

const char* socketErrorHint(int code) {
  switch (code) {
    case 10048:  // WSAEADDRINUSE
      return "地址或端口已被占用（请更换 tcp.port 或结束占用进程，"
             "可用 netstat -ano | findstr :端口 查看）";
    case 10049:  // WSAEADDRNOTAVAIL
      return "绑定地址不是本机网卡地址（请使用 0.0.0.0 或 127.0.0.1）";
    case 10013:  // WSAEACCES
      return "权限不足或防火墙阻止绑定该地址/端口";
    case 10022:  // WSAEINVAL
      return "套接字参数无效";
    case 10093:  // WSANOTINITIALISED
      return "Winsock 未初始化";
    case 10047:  // WSAEAFNOSUPPORT
      return "地址族与协议不匹配";
    default:
      return nullptr;
  }
}

#ifdef _WIN32
std::string wideToUtf8(const std::wstring& w) {
  if (w.empty()) {
    return {};
  }
  const int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(),
                                    static_cast<int>(w.size()), nullptr, 0,
                                    nullptr, nullptr);
  if (n <= 0) {
    return {};
  }
  std::string out(static_cast<std::size_t>(n), '\0');
  WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()),
                      out.data(), n, nullptr, nullptr);
  return out;
}

std::string trimMessage(std::string s) {
  while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || s.back() == ' ')) {
    s.pop_back();
  }
  return s;
}

std::string win32SystemMessageUtf8(unsigned long code) {
  wchar_t* buf = nullptr;
  const DWORD flags =
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS;
  const DWORD n = FormatMessageW(flags, nullptr, code,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 reinterpret_cast<LPWSTR>(&buf), 0, nullptr);
  if (n == 0 || buf == nullptr) {
    return {};
  }
  std::wstring w(buf, n);
  LocalFree(buf);
  return trimMessage(wideToUtf8(w));
}
#endif

}  // namespace

std::string format_boost_error(const boost::system::error_code& ec) {
  std::ostringstream os;
  os << "错误码 " << ec.value();
  if (const char* hint = socketErrorHint(ec.value())) {
    os << ": " << hint;
    return os.str();
  }
#ifdef _WIN32
  if (ec.category() == boost::system::system_category()) {
    const std::string sys = win32SystemMessageUtf8(
        static_cast<unsigned long>(ec.value()));
    if (!sys.empty()) {
      os << ": " << sys;
      return os.str();
    }
  }
#endif
  os << ": " << ec.message();
  return os.str();
}

}  // namespace util
