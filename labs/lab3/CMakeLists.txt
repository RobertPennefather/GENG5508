cmake_minimum_required(VERSION 3.8)
project(lab3)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c try.c 6k.c nopein4k.c)
add_executable(lab3 ${SOURCE_FILES})