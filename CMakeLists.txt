cmake_minimum_required(VERSION 3.16)
project(leetcode-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
#  自动发现每个题目目录里的 main.cpp，各自编译成一个可执行文件。
#
#  · 不写死任何编译器的绝对路径 —— 全部从 PATH 探测，
#    所以在任何装了 g++ 的机器上都能直接构建。
#  · CONFIGURE_DEPENDS 让 CMake 每次构建前重新扫描目录，
#    以后新增题目（只要目录里有 main.cpp）不用手动重跑配置。
#  · 目标名：目录 "15.threeSum"  ->  目标 "p15_threeSum"
#  · 产物：  每个题目目录下的 main.exe，与 VS Code 任务保持一致
# ============================================================

file(GLOB problem_sources CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*/main.cpp")

foreach(src IN LISTS problem_sources)
    get_filename_component(dir  "${src}" DIRECTORY)
    get_filename_component(name "${dir}" NAME)

    # CMake 目标名不适合带点号，把 "15.threeSum" 变成 "p15_threeSum"
    string(REPLACE "." "_" target "p${name}")
    add_executable(${target} "${src}")

    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "main"
        RUNTIME_OUTPUT_DIRECTORY "${dir}")
endforeach()

list(LENGTH problem_sources problem_count)
message(STATUS "leetcode-cpp: 已注册 ${problem_count} 道题目")
