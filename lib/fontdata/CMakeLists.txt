cmake_minimum_required(VERSION 3.9)

project(fontdata LANGUAGES CXX)

add_library(${PROJECT_NAME}
    fontdata.cpp
    fontdata.h
)

target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES} GSL)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FREETYPE_INCLUDE_DIRS})
