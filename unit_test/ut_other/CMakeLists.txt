cmake_minimum_required(VERSION 3.8)
project(ut_other)

set(CMAKE_CXX_STANDARD 20)

include_directories(../gtest/include)
link_directories(../gtest/)
set(SOURCE_FILES ut_other.cpp)
add_executable(ut_other ${SOURCE_FILES})
target_link_libraries(ut_other gtest gtest_main)
