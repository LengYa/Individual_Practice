cmake_minimum_required(VERSION 3.0)     # 指定CMake的最低版本要求

PROJECT(async_pool CXX)     # 项目名称和使用的语言

set(CMAKE_CXX_STANDARD 17)     # 设置C++标准为17
set(SRC_LIST main.cpp asyncConnPool.cpp)     # 设置源文件列表

# 添加可执行文件目标
add_executable(async_pool ${SRC_LIST})

# 设置包含目录
target_include_directories(async_pool PUBLIC "/usr/include/mysql-cppconn/jdbc")

# 设置链接库
target_link_libraries(async_pool mysqlcppconn pthread)

# 添加编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Configuring for Debug build...")
    set_target_properties(async_pool PROPERTIES COMPILE_OPTIONS "-ggdb -Wall -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Configuring for Release build...")
    set_target_properties(async_pool PROPERTIES COMPILE_OPTIONS "-Wall -O2 -DNDEBUG")
else()
    message(STATUS "No build type specified, using default settings.")
endif()

# 设置安装路径
install(TARGETS async_pool DESTINATION "${CMAKE_SOURCE_DIR}/../bin")
