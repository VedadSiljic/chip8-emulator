cmake_minimum_required(VERSION 4.0)
project(chip-8)

set(CMAKE_CXX_STANDARD 20)

add_executable(chip-8 main.cpp
    chip-8.cpp
    chip-8.h
)

find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(chip-8 PRIVATE SDL3::SDL3)
