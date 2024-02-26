# 设置CMake最小版本要求
cmake_minimum_required(VERSION 3.15)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_COMPILER "$ENV{CC}")
set(CMAKE_CXX_COMPILER "$ENV{CXX}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

# 设置项目名称
project(fpe)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

# 定义一个可执行文件
add_executable(fpe src/main.cpp src/fpe.cpp)

# 引入静态库
add_library(ubiqfpe STATIC IMPORTED)
set_target_properties(ubiqfpe PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/libubiqfpe.a)

# 链接静态库到执行文件
target_include_directories(fpe PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(fpe m crypto gmp unistring ubiqfpe)