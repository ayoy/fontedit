cmake_minimum_required(VERSION 3.9)

project(qgraphicstest LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(qgraphicstest MACOSX_BUNDLE
    focuswidget.h
    focuswidget.cpp
    glyphwidget.h
    glyphwidget.cpp
    graphicsview.h
    graphicsview.cpp
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    pixelitem.h
    pixelitem.cpp
)

target_link_libraries(qgraphicstest PRIVATE Qt5::Widgets Qt5::Core font2bytes GSL)
