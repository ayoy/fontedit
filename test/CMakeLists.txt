cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(UNIT_TESTS
    f2b_qt_compat_test.cpp
    sourcecodegeneration_test.cpp
    )

set(TARGET_NAME fontedit_app_tests)

add_executable(${TARGET_NAME}
    ${UNIT_TESTS})

target_link_libraries(${TARGET_NAME} PUBLIC Qt6::Widgets Qt6::Core appbundle gtest_main)
target_compile_definitions(${TARGET_NAME} PRIVATE ASSETS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/assets\")

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
