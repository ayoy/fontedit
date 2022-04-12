cmake_minimum_required(VERSION 3.9)

project(ui LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_library(ui
    aboutdialog.cpp
    aboutdialog.h
    aboutdialog.ui
    batchpixelchange.h
    facewidget.cpp
    facewidget.h
    focuswidget.cpp
    focuswidget.h
    glyphgraphicsview.cpp
    glyphgraphicsview.h
    glyphinfowidget.cpp
    glyphinfowidget.h
    glyphwidget.cpp
    glyphwidget.h
)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0)
endif()

target_link_libraries(ui PRIVATE Qt6::Widgets Qt6::Core common font2bytes)
