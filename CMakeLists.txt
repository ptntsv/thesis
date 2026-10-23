cmake_minimum_required(VERSION 3.15)
project(thesis)

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

add_library(checker_core STATIC checker.cpp)

target_include_directories(checker_core PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(checker_core SYSTEM PUBLIC ${SPOT_INCLUDE_DIR})
target_link_libraries(checker_core PUBLIC ${SPOT_LIB} ${BDDX_LIB})

add_executable(checker main.cpp)
target_link_libraries(checker PRIVATE checker_core)

option(CHECKER_BUILD_TESTS "Build unit tests (downloads Catch2)" ON)
if(CHECKER_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

    add_executable(tests tests/test_checker.cpp)
    target_link_libraries(tests PRIVATE checker_core Catch2::Catch2WithMain)

    enable_testing()
    include(Catch)
    catch_discover_tests(tests)
endif()
