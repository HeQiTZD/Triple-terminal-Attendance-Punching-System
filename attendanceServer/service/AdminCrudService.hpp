#pragma once

#include <string>

namespace service {

class DbExecutor;

struct AuditContext {
  int user_id = 0;
  std::string employee_id;
  std::string ip_address;
};

}  // namespace service

// 聚合头：包含所有子服务头文件
// 现有调用方无需修改 include
#include "service/admin/PersonService.hpp"
#include "service/admin/AttendanceCrudService.hpp"
#include "service/admin/DeviceCrudService.hpp"
#include "service/admin/UserService.hpp"
#include "service/admin/RoleService.hpp"
#include "service/admin/FaceCrudService.hpp"
