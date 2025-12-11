cmake_minimum_required(VERSION 3.28)
project(aoc25)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-missing-field-initializers)
add_compile_options(-fno-omit-frame-pointer)
add_compile_options(-fdiagnostics-color=always)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-error=unknown-warning-option)
endif()

add_compile_options(-fconstexpr-ops-limit=1000000000)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
# todo: had  to use gcc because clang's stl sucks, but no asan on apple's gcc -- figure out later
#    if(NOT WITH_SANITIZERS OR WITH_SANITIZERS STREQUAL "")
#        set(WITH_SANITIZERS address,undefined)
#    endif()
#
#    add_compile_options(-fsanitize=${WITH_SANITIZERS})
#    add_compile_options(-fno-optimize-sibling-calls)
#    add_link_options(-fsanitize=${WITH_SANITIZERS})
    add_definitions(-D_GLIBCXX_ASSERTIONS)
    if(WITH_SANITIZERS MATCHES "address")
        add_compile_options(-fsanitize-address-use-after-scope)
    endif()
    add_compile_options(-ftrivial-auto-var-init=pattern)
else ()
    add_compile_options(-ftrivial-auto-var-init=zero)
endif()

find_package(fmt REQUIRED)
find_package(ctre REQUIRED)
#find_package(range-v3 REQUIRED) ## cartesian_product

add_subdirectory(util)
add_subdirectory(day01)
add_subdirectory(day02)
add_subdirectory(day03)
add_subdirectory(day04)
add_subdirectory(day05)
add_subdirectory(day06)
add_subdirectory(day07)
add_subdirectory(day08)
add_subdirectory(day09)
add_subdirectory(day10)