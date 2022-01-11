cmake_minimum_required(VERSION 3.8)
project(lab7)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c test.c t2.c)
add_executable(lab7 ${SOURCE_FILES})