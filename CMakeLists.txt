cmake_minimum_required(VERSION 3.28)
project(aoc25)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-missing-field-initializers)
add_compile_options(-fno-omit-frame-pointer)
add_compile_options(-fdiagnostics-color=always)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-error=unknown-warning-option)
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(NOT WITH_SANITIZERS OR WITH_SANITIZERS STREQUAL "")
        set(WITH_SANITIZERS address,undefined)
    endif()

    add_compile_options(-fsanitize=${WITH_SANITIZERS})
    add_compile_options(-fno-optimize-sibling-calls)
    add_link_options(-fsanitize=${WITH_SANITIZERS})
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

add_subdirectory(day01)
add_subdirectory(day02)