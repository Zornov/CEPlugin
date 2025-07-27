cmake_minimum_required(VERSION 3.15)
project(CEPlugin)

set(CMAKE_CXX_STANDARD 17)

include_directories("ext/CheatEngine")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LUA_LIB "${CMAKE_SOURCE_DIR}/ext/CheatEngine/lua53-64.lib")
else()
    set(LUA_LIB "${CMAKE_SOURCE_DIR}/ext/CheatEngine/lua53-32.lib")
endif()

add_library(CEPlugin SHARED src/plugin.cpp)

target_compile_definitions(CEPlugin PRIVATE
        -D_WINDLL
        -D_USRDLL
)

set_target_properties(CEPlugin PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
)

target_link_libraries(CEPlugin
        ${LUA_LIB}
)