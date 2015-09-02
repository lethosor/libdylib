CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)
PROJECT(libdylib)

IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   MESSAGE(FATAL_ERROR "In-source builds are not allowed.")
ENDIF()

SET_SOURCE_FILES_PROPERTIES(libdylib.h PROPERTIES HEADER_FILE_ONLY TRUE)
ADD_LIBRARY(libdylib STATIC libdylib.c libdylib.h)
SET_TARGET_PROPERTIES(libdylib PROPERTIES PREFIX "")
ADD_LIBRARY(libdylibxx STATIC libdylibxx.cpp)
SET_TARGET_PROPERTIES(libdylibxx PROPERTIES PREFIX "")

OPTION(BUILD_TESTS BOOL OFF)
IF(BUILD_TESTS)
    INCLUDE_DIRECTORIES(.)
    ADD_EXECUTABLE(c-tests tests/c-tests.c)
    TARGET_LINK_LIBRARIES(c-tests libdylib)

    ADD_EXECUTABLE(cpp-tests tests/cpp-tests.cpp)
    TARGET_LINK_LIBRARIES(cpp-tests libdylibxx)

    ADD_LIBRARY(testlib SHARED tests/lib.c)
    SET_TARGET_PROPERTIES(testlib PROPERTIES PREFIX "" SUFFIX ".dylib") # For consistency, use "testlib.dylib"
ENDIF(BUILD_TESTS)
