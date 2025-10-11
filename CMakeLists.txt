cmake_minimum_required(VERSION 3.15)
project(CEPlugin)

set(CMAKE_CXX_STANDARD 17)

include_directories("ext/CheatEngine")
include_directories("ext/json")

find_package(cpr CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LUA_LIB "${CMAKE_SOURCE_DIR}/ext/CheatEngine/lib/lua53-64.lib")
else()
    set(LUA_LIB "${CMAKE_SOURCE_DIR}/ext/CheatEngine/lib/lua53-32.lib")
endif()

add_library(CEPlugin SHARED
        src/plugin.cpp
        src/hooks/process.cpp
        src/hooks/memory.cpp
)

target_compile_definitions(CEPlugin PRIVATE
        -D_WINDLL
        -D_USRDLL
)

set_target_properties(CEPlugin PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
)

target_link_libraries(CEPlugin
        PRIVATE
        ${LUA_LIB}
        cpr::cpr
)