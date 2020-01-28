cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(font2bytes)

find_package(Freetype REQUIRED)

add_subdirectory(fontdata)

set(HEADERS
    bytewriter.h
    convertererror.h
    f2b.h
    fixedwidthfontconverter.h
    inputimage.h
    sourcecodegenerator.h
    )

add_library(${PROJECT_NAME}
    SHARED
    ${HEADERS}
    bytewriter.cpp
    convertererror.cpp
    fixedwidthfontconverter.cpp
    )

target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES} fontdata GSL)
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
    ${FREETYPE_INCLUDE_DIRS})


set_target_properties(${PROJECT_NAME} PROPERTIES
        SOVERSION 1.0.0
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

INSTALL(FILES ${HEADERS} DESTINATION include/f2b)
INSTALL(FILES include/f2b.h DESTINATION include)
INSTALL(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
