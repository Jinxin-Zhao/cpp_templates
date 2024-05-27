cmake_minimum_required(VERSION 3.8)
project(ut_chapter_05)

set(CMAKE_CXX_STANDARD 17)

include_directories(../gtest/include)
link_directories(../gtest/)
set(SOURCE_FILES ut_chapter_05.cpp)
add_executable(ut_chapter_05 ${SOURCE_FILES})
target_link_libraries(ut_chapter_05 gtest gtest_main)