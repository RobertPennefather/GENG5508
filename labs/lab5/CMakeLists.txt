cmake_minimum_required(VERSION 3.8)
project(lab5)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c lab-gf.c lab5.c)
add_executable(lab5 ${SOURCE_FILES})