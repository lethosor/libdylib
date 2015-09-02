cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(libdylib)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "In-source builds are not allowed.")
endif()

set_source_files_properties(libdylib.h PROPERTIES HEADER_FILE_ONLY TRUE)
add_library(libdylib STATIC libdylib.c libdylib.h)
set_target_properties(libdylib PROPERTIES PREFIX "")
add_library(libdylibxx STATIC libdylibxx.cpp)
set_target_properties(libdylibxx PROPERTIES PREFIX "")

option(BUILD_TESTS BOOL OFF)
if(BUILD_TESTS)
    include_directories(.)
    add_executable(c-tests tests/c-tests.c)
    target_link_libraries(c-tests libdylib)

    add_executable(cpp-tests tests/cpp-tests.cpp)
    target_link_libraries(cpp-tests libdylibxx)

    add_library(testlib SHARED tests/lib.c)
    set_target_properties(testlib PROPERTIES PREFIX "" SUFFIX ".dylib") # For consistency, use "testlib.dylib"

    add_library(ptestlib SHARED tests/lib.c)
endif(BUILD_TESTS)
