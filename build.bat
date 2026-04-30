@echo off
set BUILD_DIR=build

if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

echo --- Running CMake Configuration ---
set CC=clang
set CXX=clang++

cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 exit /b %errorlevel%

echo --- Building ---
cmake --build .
if %errorlevel% neq 0 exit /b %errorlevel%

echo --- Done! ---
cd ..