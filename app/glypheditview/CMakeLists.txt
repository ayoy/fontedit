cmake_minimum_required(VERSION 3.9)

project(glypheditview LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_library(glypheditview
    focuswidget.h
    focuswidget.cpp
    glyphgraphicsview.h
    glyphgraphicsview.cpp
    glyphwidget.h
    glyphwidget.cpp
    pixelwidget.h
    pixelwidget.cpp
)

target_link_libraries(glypheditview PRIVATE Qt5::Widgets Qt5::Core font2bytes)
