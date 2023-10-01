cmake_minimum_required(VERSION 3.16)

project(lbfc)

add_executable(lbfc src/main.c)

set_property(TARGET lbfc PROPERTY C_STANDARD 99)
