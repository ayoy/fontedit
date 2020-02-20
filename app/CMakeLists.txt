cmake_minimum_required(VERSION 3.9)

if (UNIX AND NOT APPLE)
    project(fontedit LANGUAGES CXX)
else()
    project(FontEdit LANGUAGES CXX)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_subdirectory(common)
add_subdirectory(glypheditview)
add_subdirectory(facepreview)

set(SRC_FILES
    assets.qrc
    command.h
    fontfaceviewmodel.cpp
    fontfaceviewmodel.h
    global.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    mainwindowmodel.cpp
    mainwindowmodel.h
    )

if (APPLE)
    message(STATUS "Building MacOS X Bundle")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRC_FILES})
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/macos/Info.plist)
else()
    add_executable(${PROJECT_NAME} ${SRC_FILES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE common glypheditview facepreview)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Core common glypheditview facepreview font2bytes GSL)
