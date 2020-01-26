cmake_minimum_required(VERSION 3.9)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

project(fontdata LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_library(${PROJECT_NAME}
    fontdata.cpp
    fontdata.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Core)
