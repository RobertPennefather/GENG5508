cmake_minimum_required(VERSION 3.8)
project(lab8)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c getdata.c try.c)
add_executable(lab8 ${SOURCE_FILES})