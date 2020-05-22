cmake_minimum_required(VERSION 3.9)

if (UNIX AND NOT APPLE)
    project(fontedit LANGUAGES CXX)
else()
    project(FontEdit LANGUAGES CXX)
endif()

set(APP_TARGET_NAME ${PROJECT_NAME} PARENT_SCOPE)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)
find_package(Qt5LinguistTools)

add_subdirectory(utf8)
add_subdirectory(common)
add_subdirectory(ui)

set(SRC_FILES
    addglyphdialog.cpp
    addglyphdialog.h
    addglyphdialog.ui
    command.h
    fontfaceviewmodel.cpp
    fontfaceviewmodel.h
    global.h
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    mainwindowmodel.cpp
    mainwindowmodel.h
    qfontfacereader.cpp
    qfontfacereader.h
    semver.hpp
    sourcecoderunnable.cpp
    sourcecoderunnable.h
    updatemanager.cpp
    updatemanager.h
    )

add_library(appbundle ${SRC_FILES})
target_include_directories(appbundle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} utf8 common ui)
target_link_libraries(appbundle PUBLIC Qt5::Widgets Qt5::Core Qt5::Network common ui font2bytes GSL)

if (APPLE)
    message(STATUS "Building MacOS X Bundle")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        main.cpp
        assets.qrc
        )

    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/macos/Info.plist)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE fontedit.icns)

    set(RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources)
    file(MAKE_DIRECTORY ${RESOURCES_DIR})
    file(COPY ${PROJECT_SOURCE_DIR}/macos/fontedit.icns DESTINATION ${RESOURCES_DIR})
elseif(WIN32)
    add_executable(${PROJECT_NAME} WIN32
        main.cpp
        assets.qrc
        win/fontedit.rc
        )
else()
    add_executable(${PROJECT_NAME}
        main.cpp
        assets.qrc
        )
    if (UNIX)
        install(TARGETS ${PROJECT_NAME}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib)
        install(FILES x11/fontedit.desktop DESTINATION share/applications)
        install(DIRECTORY x11/icons DESTINATION share)

        # uninstall target
        if(NOT TARGET uninstall)
            configure_file(
                "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
                "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
                IMMEDIATE @ONLY)

            add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
        endif()

    endif()
endif()

target_include_directories(${PROJECT_NAME} PRIVATE appbundle)
target_link_libraries(${PROJECT_NAME} PRIVATE appbundle)

target_compile_definitions(${PROJECT_NAME} PRIVATE VERSION="${APP_VERSION}" BUILD="${APP_BUILD}" YEAR="${APP_YEAR}")
target_compile_definitions(appbundle PRIVATE VERSION="${APP_VERSION}" BUILD="${APP_BUILD}" YEAR="${APP_YEAR}")
target_compile_definitions(ui PRIVATE VERSION="${APP_VERSION}" BUILD="${APP_BUILD}" YEAR="${APP_YEAR}")
