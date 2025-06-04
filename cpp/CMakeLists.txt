cmake_minimum_required(VERSION 3.10)
project(OpenFrontPrototype)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(OpenFrontPrototype src/main.cpp)

target_link_libraries(OpenFrontPrototype SDL2::SDL2)
