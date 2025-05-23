cmake_minimum_required(VERSION 3.28)
project(ex1)

include(../../nanopi.cmake)
add_executable(ex1 safesleep.c ex1.c)