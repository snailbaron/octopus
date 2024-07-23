cmake_minimum_required(VERSION 3.21)

project(octopus)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(deps)

if(MSVC)
    add_compile_options(/W4 /WX /w44061 /w44062)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)