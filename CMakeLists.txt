cmake_minimum_required(VERSION 3.9)
project(FontEdit)

set(APP_VERSION 1.0.0-rc1)
set(APP_BUILD 1)
set(APP_YEAR 2020)

set(BUILD_TESTS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

enable_testing()
add_subdirectory(gsl EXCLUDE_FROM_ALL)
add_subdirectory(lib EXCLUDE_FROM_ALL)
add_subdirectory(app)
if(${BUILD_TESTS})
    add_subdirectory(test)
endif()

# Download and unpack googletest at configure time

include(googletest.cmake)

#set_target_properties(GSL PROPERTIES EXCLUDE_FROM_ALL TRUE)
