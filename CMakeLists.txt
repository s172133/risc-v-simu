cmake_minimum_required(VERSION 3.17.5)
project(RISC_V C)

set(CMAKE_C_STANDARD 11)

add_executable(RISC_V
        main.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/binary.txt
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
