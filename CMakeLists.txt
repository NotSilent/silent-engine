cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(sengine)

add_subdirectory(external)
add_subdirectory(libs)

add_executable(sengine src/main.cpp)

target_link_libraries(sengine base)
set_target_properties(sengine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)