cmake_minimum_required(VERSION 4.2)
project(CEPlugin)

set(CMAKE_CXX_STANDARD 26)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH "64")
else()
    set(ARCH "32")
endif()

set(CE_DIR "${CMAKE_SOURCE_DIR}/ext/CheatEngine")

include(FetchContent)
FetchContent_Declare(websocketpp
        GIT_REPOSITORY https://github.com/zaphoyd/websocketpp
        GIT_TAG 0.8.2
        GIT_SHALLOW TRUE
)
FetchContent_Declare(asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio
        GIT_TAG asio-1-36-0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(websocketpp asio)

file(GLOB_RECURSE SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} CEPlugin.def)

target_include_directories(${PROJECT_NAME} PRIVATE
        src
        ${CE_DIR}
        ${websocketpp_SOURCE_DIR}
        ${asio_SOURCE_DIR}/asio/include
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        ASIO_STANDALONE
        _WEBSOCKETPP_CPP11_STL_
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        $<$<CONFIG:Release>:/O2>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CE_DIR}/lib/lua53-${ARCH}.lib
        ws2_32
)