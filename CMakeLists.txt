cmake_minimum_required(VERSION 3.21)
project(sengine)

add_subdirectory(external)

add_subdirectory(libs)

add_subdirectory(src)

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR}/bin)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/bin/sengine.ini)
    file(WRITE ${CMAKE_BINARY_DIR}/bin/sengine.ini)
endif ()