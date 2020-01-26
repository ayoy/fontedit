cmake_minimum_required(VERSION 3.9)

project(qgraphicstest LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_subdirectory(fontdata)
add_subdirectory(glypheditview)

set(SRC_FILES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    )

if (APPLE AND (NOT CMAKE_BUILD_TYPE STREQUAL "Debug"))
    message(STATUS "Using MacOS X Bundle (release)")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRC_FILES})
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/macos/Info.plist)
else()
    add_executable(${PROJECT_NAME} ${SRC_FILES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE glypheditview)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Core glypheditview font2bytes GSL)
