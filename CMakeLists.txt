cmake_minimum_required(VERSION 3.9)
project(FontEdit)

set(BUILD_TESTS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

enable_testing()
add_subdirectory(gsl)
add_subdirectory(lib)
add_subdirectory(app)


# Download and unpack googletest at configure time

include(googletest.cmake)
