cmake_minimum_required(VERSION 3.9)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

project(utf8 LANGUAGES CXX)

add_library(utf8 INTERFACE)

target_include_directories(utf8 SYSTEM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
