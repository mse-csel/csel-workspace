cmake_minimum_required(VERSION 3.28)
project(led-controller)

include(../nanopi.cmake)
add_executable(led-controller main.c)
