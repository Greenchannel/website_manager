cmake_minimum_required(VERSION 3.14)

project(webm
    VERSION 1.0.0
    DESCRIPTION "A simple website manager"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 统一输出到 build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# CLI11.hpp 与 spdlog/ 都是仓库根目录下的单头文件依赖，直接 -I 根目录即可
add_executable(webm webm.cpp)

target_include_directories(webm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Windows（MinGW）下静态链接，产出不依赖 libstdc++-6.dll / libwinpthread-1.dll 的单文件 exe
target_link_options(webm PRIVATE
    $<$<AND:$<BOOL:${WIN32}>,$<CXX_COMPILER_ID:GNU>>:-static;-static-libgcc;-static-libstdc++>
)

target_compile_options(webm PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)
