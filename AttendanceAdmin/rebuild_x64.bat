@echo off
set "VSCMD_START_DIR=%CD%"
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvarsall.bat" amd64
if errorlevel 1 (
    echo ERROR: vcvarsall.bat failed
    exit /b 1
)
echo === Compiler ===
where cl.exe
echo === LIB ===
echo %LIB%
cd /d E:\project\AttendanceServer
cmake -S . -B out\build\debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="C:\Qt\6.10.2\msvc2022_64" 2>&1
if errorlevel 1 (
    echo ERROR: cmake configure failed
    exit /b 1
)
cmake --build out\build\debug 2>&1
if errorlevel 1 (
    echo ERROR: cmake build failed
    exit /b 1
)
echo BUILD SUCCESS
