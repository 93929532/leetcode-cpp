cmake_minimum_required(VERSION 3.16)
project(leetcode-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
#  源码编码：全仓 main.cpp 都是「UTF-8 无 BOM」，而注释和测试用例里
#  都有中文。MSVC 不加选项时按系统本地代码页解析源文件（中文 Windows
#  上是 936/GBK），会把 UTF-8 中文字节的边界切错，进而把字节 0x22 误
#  认成字符串的结束引号 —— 一条中文注释就能引发几十条 C2143 / C2001 /
#  C3688 语法错误。g++ 从源码探测编码，通常不会踩这个坑。
#
#  这里按编译器分别显式指定 UTF-8（对另一方的开关无效也无害）：
#    · MSVC : /utf-8 等价于 /source-charset:utf-8 /execution-charset:utf-8
#             /EHsc 是标准异常模型，MSVC 下用 <iostream> 不带它会告警 C4530
#    · g++  : 两个 -f*charset 开关把输入与执行字符集都钉死为 UTF-8
#  这样同一份源码在 MinGW(g++) 和 MSVC(cl) 下都能正确编译。
# ============================================================
if(MSVC)
    add_compile_options(/utf-8 /EHsc)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

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
