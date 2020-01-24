cmake_minimum_required(VERSION 3.9)

project(qgraphicstest LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_subdirectory(glypheditview)

add_executable(qgraphicstest MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

target_include_directories(qgraphicstest PRIVATE glypheditview)

target_link_libraries(qgraphicstest PRIVATE Qt5::Widgets Qt5::Core glypheditview font2bytes GSL)
