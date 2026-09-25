cmake_minimum_required(VERSION 3.14)

# 项目名称
project(webm
    VERSION 1.0.0
    DESCRIPTION "A simple website manager"
    LANGUAGES CXX C
)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录（统一放到 build 下的 bin 目录）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# 添加 monocypher 源文件 + webm.cpp
add_executable(webm
    webm.cpp
    monocypher/src/monocypher.c
)

# 包含头文件目录（CLI11.hpp、spdlog/、monocypher/）
target_include_directories(webm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/monocypher/src
)

# 链接 Windows 系统库（用于随机数生成等）
if(WIN32)
    target_link_libraries(webm PRIVATE
        advapi32
    )
endif()

# 编译选项
target_compile_options(webm PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)