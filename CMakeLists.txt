cmake_minimum_required(VERSION 3.16)

project(lbfc)

add_executable(lbfc src/main.c src/oplist.c src/bracket_matcher_stack.c src/platforms/nasm_x86_64_linux.c)

set_property(TARGET lbfc PROPERTY C_STANDARD 99)
