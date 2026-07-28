#pragma once

#include <boost/system/error_code.hpp>

#include <string>

namespace util {

// 将 Boost/system 错误码格式化为 UTF-8 说明（含数值与中文提示，避免 Windows 乱码）。
std::string format_boost_error(const boost::system::error_code& ec);

}  // namespace util
