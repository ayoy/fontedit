cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(font2bytes)

set(HEADERS
    f2b.h
    fontdata.h
    fontsourcecodegenerator.h
    format.h
    sourcecode.h
    )

set(SOURCES
    fontdata.cpp
    fontsourcecodegenerator.cpp
    )

if (LIBFONTEDIT_STANDALONE_PROJECT)
    add_library(${PROJECT_NAME}
        SHARED
        ${HEADERS}
        ${SOURCES}
        )
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        SOVERSION 1.1.0
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

else ()
    add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
endif ()

target_link_libraries(${PROJECT_NAME} GSL)
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
    )

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0)
elseif (UNIX)
    if (LIBFONTEDIT_STANDALONE_PROJECT)
        INSTALL(FILES ${HEADERS} DESTINATION include/f2b)
        INSTALL(FILES include/f2b.h DESTINATION include)

        INSTALL(TARGETS ${PROJECT_NAME} 
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)
    endif ()
endif ()

