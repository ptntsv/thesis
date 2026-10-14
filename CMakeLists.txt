cmake_minimum_required(VERSION 3.15)
project(thesis)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "-Wall -g -fsanitize=address")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SPOT_ROOT "${CMAKE_SOURCE_DIR}/spot-2.16/install")

find_path(SPOT_INCLUDE_DIR spot/tl/formula.hh HINTS "${SPOT_ROOT}/include")

find_library(SPOT_LIB NAMES spot HINTS "${SPOT_ROOT}/lib")
find_library(BDDX_LIB NAMES bddx HINTS "${SPOT_ROOT}/lib")

if(NOT SPOT_INCLUDE_DIR OR NOT SPOT_LIB)
    message(FATAL_ERROR "Spot didn't found in ${SPOT_ROOT}")
endif()

add_executable(checker main.cpp)

target_include_directories(checker SYSTEM PRIVATE ${SPOT_INCLUDE_DIR})
target_link_libraries(checker PRIVATE ${SPOT_LIB} ${BDDX_LIB})
