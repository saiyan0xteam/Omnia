cmake_minimum_required(VERSION 3.15)
project(Omnia VERSION 0.0.3 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    main.cpp
    Library/EasyCPP/Logger.cpp
    Class/CModLoader.cpp
    Library/Discord/Discord.cpp
    Library/Discord/DiscordSDK/src/discord_rpc.cpp
    Library/Discord/DiscordSDK/src/rpc_connection.cpp
    Library/Discord/DiscordSDK/src/serialization.cpp
    Library/miniz/miniz.c
)

if(WIN32)
    set(SOURCES ${SOURCES}
        Library/Discord/DiscordSDK/src/connection_win.cpp
        Library/Discord/DiscordSDK/src/discord_register_win.cpp
    )
else()
    set(SOURCES ${SOURCES}
        Library/Discord/DiscordSDK/src/connection_unix.cpp
        Library/Discord/DiscordSDK/src/discord_register_linux.cpp
    )
endif()

if(WIN32)
    set(SOURCES ${SOURCES} Omnia.rc)
endif()

# Headers (for IDE visibility)
set(HEADERS
    shared.h
    Library/Discord/Discord.h
    Library/EasyCPP/Logger.h
)

# Python & Lua Support
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Lua Source Files (excluding lua.c and luac.c which are for standalone executables)
file(GLOB LUA_SOURCES "Library/Lua/src/*.c")
list(REMOVE_ITEM LUA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Library/Lua/src/lua.c" "${CMAKE_CURRENT_SOURCE_DIR}/Library/Lua/src/luac.c")
add_library(lua_lib STATIC ${LUA_SOURCES})

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Library
    ${CMAKE_CURRENT_SOURCE_DIR}/Class
    ${CMAKE_CURRENT_SOURCE_DIR}/Library/RayLib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Library/Discord/DiscordSDK/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Library/Discord/DiscordSDK/src
    ${CMAKE_CURRENT_SOURCE_DIR}/Library/pybind11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Library/Lua/src
    ${CMAKE_CURRENT_SOURCE_DIR}/Library/sol2
    ${CMAKE_CURRENT_SOURCE_DIR}/Library/miniz
    ${Python3_INCLUDE_DIRS}
)

# Library directories (Local Raylib)
if(WIN32)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/Library/RayLib/lib)
endif()

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries (Direct linking)
if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        raylib
        lua_lib
        Python3::Python
        opengl32
        gdi32
        winmm
        shell32
    )
else()
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        raylib
        lua_lib
        Python3::Python
        GL
        m
        pthread
        dl
        rt
        X11
    )
endif()

# Build configurations
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()