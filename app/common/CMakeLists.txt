cmake_minimum_required(VERSION 3.9)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

project(common LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_library(common
    common.h
    f2b_qt_compat.cpp
    f2b_qt_compat.h)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(common PRIVATE Qt5::Widgets Qt5::Core font2bytes)
