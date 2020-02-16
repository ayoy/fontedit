cmake_minimum_required(VERSION 3.9)

project(glypheditview LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_library(glypheditview
    glyphgraphicsview.cpp
    glyphgraphicsview.h
    glyphwidget.cpp
    glyphwidget.h
)

target_link_libraries(glypheditview PRIVATE Qt5::Widgets Qt5::Core common font2bytes)
