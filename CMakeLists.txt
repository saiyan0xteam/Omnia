cmake_minimum_required(VERSION 3.15)
project(Omnia VERSION 0.0.3 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    main.cpp
    Library/EasyCPP/Logger.cpp
    Library/Discord/Discord.cpp
    Library/Discord/DiscordSDK/src/discord_rpc.cpp
    Library/Discord/DiscordSDK/src/rpc_connection.cpp
    Library/Discord/DiscordSDK/src/serialization.cpp
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

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Library
    ${CMAKE_CURRENT_SOURCE_DIR}/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/Library/RayLib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Library/Discord/DiscordSDK/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Library/Discord/DiscordSDK/src
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
        opengl32
        gdi32
        winmm
        shell32
    )
else()
    # Linux için gerekli bağlantılar (Paket yöneticisinden kurulu olduğu varsayılır veya Library içine eklenir)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        raylib
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