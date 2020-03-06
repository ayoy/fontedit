cmake_minimum_required(VERSION 3.9)
project(FontEdit)

set(CMAKE_PROJECT_VERSION_MAJOR 1)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(CMAKE_PROJECT_VERSION_PATCH 0)

set(APP_VERSION 1.0.0)
set(APP_BUILD 1)
set(APP_YEAR 2020)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_TESTS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt5 COMPONENTS Widgets REQUIRED)

enable_testing()
add_subdirectory(gtest EXCLUDE_FROM_ALL)
add_subdirectory(gsl EXCLUDE_FROM_ALL)
add_subdirectory(lib)
add_subdirectory(app)
if(${BUILD_TESTS})
    add_subdirectory(test)
endif()

if (APPLE)
    add_custom_command(OUTPUT "${APP_TARGET_NAME}.dmg"
        COMMAND macdeployqt
        ARGS "${APP_TARGET_NAME}.app" "-libpath=/lib" "-dmg"
        MAIN_DEPENDENCY ${APP_TARGET_NAME}.app
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    add_custom_target(dmg
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${APP_TARGET_NAME}.dmg"
        )
elseif (WIN32)
    message(STATUS "ADDING WINDOWS INSTALLER TARGET")
    add_custom_command(OUTPUT "win"
        COMMAND windeployqt.exe
        ARGS "${APP_TARGET_NAME}.exe" "--dir" "win"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    add_custom_target(installer
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/win"
        )
endif ()


set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "dominik@kapusta.cc")
set(CPACK_PROJECT_HOMEPAGE_URL "https://kapusta.cc")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_NAME "fontedit")
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt5-default (>= 5.9)")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
    "Edit and convert fonts to byte arrays suitable for use in embedded systems' displays")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_PREDEPENDS "cmake (>= 3.9), g++ (>= 7.4)")
include(CPack)
