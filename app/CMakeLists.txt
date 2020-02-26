cmake_minimum_required(VERSION 3.9)
project(fontedit-all)

add_subdirectory(src)

enable_testing()
if(${BUILD_TESTS})
    add_subdirectory(test)
endif()
