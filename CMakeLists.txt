cmake_minimum_required(VERSION 3.9)
project(FontEdit)

set(CMAKE_PROJECT_VERSION_MAJOR 1)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(CMAKE_PROJECT_VERSION_PATCH 0)

set(APP_VERSION 1.0.0)
set(APP_BUILD 1)
set(APP_YEAR 2020)

set(BUILD_TESTS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt5 COMPONENTS Widgets REQUIRED)

enable_testing()
add_subdirectory(gsl EXCLUDE_FROM_ALL)
add_subdirectory(lib EXCLUDE_FROM_ALL)
add_subdirectory(app)
if(${BUILD_TESTS})
    add_subdirectory(test EXCLUDE_FROM_ALL)
endif()

# Download and unpack googletest at configure time

include(googletest.cmake)

if (APPLE)
    add_custom_command(OUTPUT "${APP_TARGET_NAME}.dmg"
        COMMAND macdeployqt
        ARGS "${APP_TARGET_NAME}.app" "-libpath=/lib" "-dmg"
        MAIN_DEPENDENCY ${APP_TARGET_NAME}.app
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    add_custom_target(dmg
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${APP_TARGET_NAME}.dmg"
        )
endif()
