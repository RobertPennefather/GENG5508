cmake_minimum_required(VERSION 3.8)
project(lab4)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c try.c)
add_executable(lab4 ${SOURCE_FILES})