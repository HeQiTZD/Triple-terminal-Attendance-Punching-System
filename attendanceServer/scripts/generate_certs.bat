@echo off
REM 生成 TLS 证书脚本
REM 需要 OpenSSL 已安装并在 PATH 中

set CERT_DIR=%~dp0..\certs
if not exist "%CERT_DIR%" mkdir "%CERT_DIR%"

echo ========================================
echo 生成 CA 证书
echo ========================================

REM 生成 CA 私钥
openssl genrsa -out "%CERT_DIR%\ca.key" 4096

REM 生成 CA 证书
openssl req -new -x509 -days 3650 -key "%CERT_DIR%\ca.key" -out "%CERT_DIR%\ca.crt" -subj "/CN=Attendance System CA/O=AttendanceSystem"

echo.
echo ========================================
echo 生成服务端证书
echo ========================================

REM 生成服务端私钥
openssl genrsa -out "%CERT_DIR%\server.key" 2048

REM 生成服务端证书签名请求
openssl req -new -key "%CERT_DIR%\server.key" -out "%CERT_DIR%\server.csr" -subj "/CN=localhost/O=AttendanceSystem"

REM 使用 CA 签名服务端证书
openssl x509 -req -days 365 -in "%CERT_DIR%\server.csr" -CA "%CERT_DIR%\ca.crt" -CAkey "%CERT_DIR%\ca.key" -CAcreateserial -out "%CERT_DIR%\server.crt"

echo.
echo ========================================
echo 证书生成完成
echo ========================================
echo.
echo 证书位置: %CERT_DIR%
echo   - CA 证书: ca.crt
echo   - 服务端证书: server.crt
echo   - 服务端私钥: server.key
echo.
echo 请将 ca.crt 复制到设备端和管理端
echo.
pause
