cmake_minimum_required(VERSION 3.8)
project(ut_chapter_11)

set(CMAKE_CXX_STANDARD 20)

include_directories(../gtest/include)
link_directories(../gtest/)
set(SOURCE_FILES ut_chapter_11.cpp)
add_executable(ut_chapter_11 ${SOURCE_FILES})
target_link_libraries(ut_chapter_11 gtest gtest_main)