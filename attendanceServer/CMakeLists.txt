cmake_minimum_required(VERSION 3.10)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(attendanceServer)

# C++ standard (required for std::optional, etc.)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#设置boost库
set(BOOST_ROOT C:/boost_1.91.0_sdk)
set(Boost_LIBRARY_DIRS C:/boost_1.91.0_sdk/lib)

# 使用静态库（默认动态）
set(Boost_USE_STATIC_LIBS ON)
# 使用多线程库（默认 ON）
set(Boost_USE_MULTITHREADED ON)
# 如果使用静态C++运行时（Windows）
set(Boost_USE_STATIC_RUNTIME OFF)

# Boost.System 在 1.91 中通常为仅头文件；官方预置 SDK 往往不含 boost_system 的 CMake 包。
# Boost.Filesystem（1.91）通过 atomic + headers 拉取依赖，不再需要单独链接 Boost::system。
# Boost.Asio：header-only；含路径由 Boost_INCLUDE_DIRS 提供（需存在 boost/asio.hpp，与 BOOST_ROOT 一致）。
find_package(Boost 1.78 REQUIRED COMPONENTS regex filesystem)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

find_package(Threads REQUIRED)

message(Boost\ version: ${Boost_VERSION_STRING})
message(Boost\ include\ dir: ${Boost_INCLUDE_DIRS})
message(Boost\ library\ dir: ${Boost_LIBRARY_DIRS})

# 查找 MySQL Connector/C++
set(mysql-concpp_DIR "D:/mysql-connector-c++-9.7.0-winx64")
find_package(mysql-concpp REQUIRED)

# nlohmann/json：本仓库内 json-develop 为官方源码目录（仅头文件实现）
# add_subdirectory 会注册 INTERFACE 目标 nlohmann_json::nlohmann_json，用于传递 include 等用法要求
add_subdirectory(json-develop)

# OpenSSL（用于 TLS 和加密）
set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
find_package(OpenSSL REQUIRED)
message(OpenSSL\ version: ${OPENSSL_VERSION})
message(OpenSSL\ include\ dir: ${OPENSSL_INCLUDE_DIR})

# jwt-cpp（用于 JWT 令牌生成和验证）
set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JWT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/jwt-cpp)

set(ARCFACE_SDK_DIR "${CMAKE_SOURCE_DIR}/arcface")

add_executable(attendanceServer
    main.cpp
    config/ServerConfig.cpp
    config/ConfigFile.cpp
    config/ConsoleConfigUi.cpp
    db/DbClient.cpp
    db/Schema.cpp
    db/PersonRepository.cpp
    db/UserAccountRepository.cpp
    db/DeviceRepository.cpp
    db/AttendanceRecordRepository.cpp
    db/FaceDataRepository.cpp
    db/DeviceConfigRepository.cpp
    db/RoleRepository.cpp
    db/OperationLogRepository.cpp
    net/Framing.cpp
    net/Session.cpp
    net/TcpServer.cpp
    net/TlsSession.cpp
    protocol/LineCodec.cpp
    protocol/Envelope.cpp
    protocol/ResponseBuilders.cpp
    protocol/DbErrorMap.cpp
    protocol/GatewaySessionHandler.cpp
    service/AdminRegistry.cpp
    service/DbExecutor.cpp
    service/DeviceRegistry.cpp
    service/EventSubscriptionRegistry.cpp
    service/MessageRouter.cpp
    service/admin/PersonService.cpp
    service/admin/AttendanceCrudService.cpp
    service/admin/DeviceCrudService.cpp
    service/admin/UserService.cpp
    service/admin/RoleService.cpp
    service/admin/FaceCrudService.cpp
    service/AttendanceService.cpp
    service/DeviceService.cpp
    service/DeviceConfigService.cpp
    service/SyncService.cpp
    service/TokenManager.cpp
    util/PasswordHash.cpp
    util/SystemError.cpp
    util/ArcFaceEngine.cpp
    util/JwtUtil.cpp
    util/stb_image.h)

# 强制源文件 / 字面量 / 执行集均使用 UTF-8。
# - MSVC：/utf-8 等价于 /source-charset:utf-8 /execution-charset:utf-8，
#   避免 mysqlx 在 bind() 时报 "Failed string conversion"。
# - GCC/Clang：默认本就是 UTF-8，这里再显式声明一次。
target_compile_options(attendanceServer PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-finput-charset=UTF-8;-fexec-charset=UTF-8>
)

target_include_directories(attendanceServer PRIVATE 
                ${CMAKE_SOURCE_DIR}
                ${ARCFACE_SDK_DIR}/include               
)

target_link_libraries(attendanceServer PRIVATE
    mysql::concpp
    Boost::regex
    Boost::filesystem
    nlohmann_json::nlohmann_json  # JSON 解析与序列化；头文件 INTERFACE 目标，不生成静态/动态库
    Threads::Threads
    ${ARCFACE_SDK_DIR}/lib/libarcsoft_face_engine.lib
    OpenSSL::SSL
    OpenSSL::Crypto
    jwt-cpp::jwt-cpp
)

if(WIN32)
    # 避免 windows.h 间接包含 WinSock.h，与 Boost.Asio / winsock2.h 冲突。
    target_compile_definitions(attendanceServer PRIVATE WIN32_LEAN_AND_MEAN)
    target_link_libraries(attendanceServer PRIVATE ws2_32 mswsock crypt32)
        add_custom_command(TARGET attendanceServer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ARCFACE_SDK_DIR}/lib/libarcsoft_face_engine.dll"
        $<TARGET_FILE_DIR:attendanceServer>
    )
endif()

