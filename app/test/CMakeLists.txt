cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(UNIT_TEST_LIST
    f2b_qt_compat
    )

foreach(NAME IN LISTS UNIT_TEST_LIST)
    list(APPEND UNIT_TEST_SOURCE_LIST ${NAME}_test.cpp)
endforeach()

set(TARGET_NAME fontedit_app_tests)

add_executable(${TARGET_NAME}
    ${UNIT_TEST_SOURCE_LIST})

target_link_libraries(${TARGET_NAME} PUBLIC Qt5::Widgets Qt5::Core common gtest_main)

target_include_directories(${TARGET_NAME} PUBLIC ../../lib/src)

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
