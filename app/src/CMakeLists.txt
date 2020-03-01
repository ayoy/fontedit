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
find_package(Qt5LinguistTools)

add_subdirectory(common)
add_subdirectory(glypheditview)
add_subdirectory(facepreview)

set(TS_FILES l10n/fontedit_en.ts)
qt5_add_translation(QM_FILES ${TS_FILES})

set(SRC_FILES
    command.h
    fontfaceviewmodel.cpp
    fontfaceviewmodel.h
    global.h
    aboutdialog.cpp
    aboutdialog.h
    aboutdialog.ui
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    mainwindowmodel.cpp
    mainwindowmodel.h
    sourcecoderunnable.cpp
    sourcecoderunnable.h
    )

add_library(appbundle ${SRC_FILES})
target_include_directories(appbundle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} common glypheditview facepreview)
target_link_libraries(appbundle PUBLIC Qt5::Widgets Qt5::Core common glypheditview facepreview font2bytes GSL)

if (APPLE)
    message(STATUS "Building MacOS X Bundle")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        main.cpp
        ${QM_FILES}
        l10n/fontedit_en.ts
        assets.qrc
        )

    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/macos/Info.plist)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE fontedit.icns)

    set(RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources)
    file(MAKE_DIRECTORY ${RESOURCES_DIR})
    file(COPY ${PROJECT_SOURCE_DIR}/macos/fontedit.icns DESTINATION ${RESOURCES_DIR})
else()
    add_executable(${PROJECT_NAME}
        main.cpp
        ${QM_FILES}
        l10n/fontedit_en.ts
        assets.qrc
        )
endif()


target_include_directories(${PROJECT_NAME} PRIVATE appbundle)
target_link_libraries(${PROJECT_NAME} PRIVATE appbundle)
