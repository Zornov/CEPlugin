cmake_minimum_required(VERSION 3.19)
project(CEPlugin)

cmake_policy(SET CMP0167 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_GENERATOR_PLATFORM x64)

include_directories("ext/CheatEngine")
include_directories("ext/json")

find_package(Boost REQUIRED)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LUA_LIB "${CMAKE_SOURCE_DIR}/ext/CheatEngine/lib/lua53-64.lib")
else()
    set(LUA_LIB "${CMAKE_SOURCE_DIR}/ext/CheatEngine/lib/lua53-32.lib")
endif()

add_library(CEPlugin SHARED
        src/plugin.cpp
        src/hooks/process.cpp
        src/hooks/memory.cpp
        src/core/logging/logging.h
        src/core/logging/logging.cpp
        src/core/console/console.cpp
        src/core/console/console.h
        src/core/hooker/hooker.cpp
        src/core/hooker/hooker.h
)

target_compile_definitions(CEPlugin PRIVATE
        _WINDLL
        _USRDLL
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

set_target_properties(CEPlugin PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
)

target_link_libraries(CEPlugin
        PRIVATE
        ${LUA_LIB}
        Ws2_32
        Boost::boost
)