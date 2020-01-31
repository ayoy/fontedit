cmake_minimum_required(VERSION 3.9)

project(facepreview LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_library(facepreview
    facewidget.cpp
    facewidget.h
)

target_link_libraries(facepreview PRIVATE Qt5::Widgets Qt5::Core font2bytes)
