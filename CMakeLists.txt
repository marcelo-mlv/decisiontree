cmake_minimum_required(VERSION 3.10)
project(fgsfds C)

set(CMAKE_C_STANDARD 99)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(decisiontree ${SRC_FILES})
