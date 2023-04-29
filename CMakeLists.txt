cmake_minimum_required(VERSION 3.16)
project(IOS_Project2 C)

set(CMAKE_C_STANDARD 11)

add_executable(IOS_Project2 proj2.c helper_functions.h helper_functions.c)
target_link_libraries(IOS_Project2 pthread)