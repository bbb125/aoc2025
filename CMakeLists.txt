cmake_minimum_required(VERSION 3.28)
project(aoc25)

set(CMAKE_CXX_STANDARD 23)
find_package(fmt REQUIRED)
find_package(ctre REQUIRED)

add_subdirectory(day01)
add_subdirectory(day02)