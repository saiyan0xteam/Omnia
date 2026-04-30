#!/bin/bash

set -e

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"

echo "--- Running CMake Configuration (Linux) ---"
export CC=clang
export CXX=clang++

cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug

echo "--- Building ---"
cmake --build .

echo "--- Done! ---"
cd ..
