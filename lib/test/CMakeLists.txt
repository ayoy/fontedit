
cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(UNIT_TEST_LIST
    fontface
    glyph
    sourcecode)

foreach(NAME IN LISTS UNIT_TEST_LIST)
    list(APPEND UNIT_TEST_SOURCE_LIST ${NAME}_test.cpp)
endforeach()

set(TARGET_NAME font2bytes_tests)

add_executable(${TARGET_NAME}
    ${UNIT_TEST_SOURCE_LIST})

target_link_libraries(${TARGET_NAME} PUBLIC font2bytes gtest_main)

target_include_directories(${TARGET_NAME}
    PUBLIC ../src)

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
