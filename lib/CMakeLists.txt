cmake_minimum_required(VERSION 3.9)
project(libFontEdit)

set(LIBFONTEDIT_STANDALONE_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(LIBFONTEDIT_STANDALONE_PROJECT ON)
endif ()

add_subdirectory(src)

enable_testing()
if(${BUILD_TESTS})
    add_subdirectory(test)
endif()
