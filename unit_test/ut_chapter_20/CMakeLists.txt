cmake_minimum_required(VERSION 3.8)
project(ut_chapter_20)

set(CMAKE_CXX_STANDARD 20)

include_directories(../gtest/include)
link_directories(../gtest/)
set(SOURCE_FILES ut_chapter_20.cpp)
add_executable(ut_chapter_20 ${SOURCE_FILES})
target_link_libraries(ut_chapter_20 gtest gtest_main)
