cmake_minimum_required(VERSION 3.21)
project(sengine)

add_subdirectory(external)

add_subdirectory(libs)

add_subdirectory(src)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/bin/sengine.ini)
    file(WRITE ${CMAKE_BINARY_DIR}/bin/sengine.ini)
endif ()

find_package(Vulkan)

set(EXECUTABLE_DIR ${CMAKE_BINARY_DIR}/bin)
set(SHADERS_DIR ${EXECUTABLE_DIR}/shaders)
file(COPY shaders DESTINATION ${EXECUTABLE_DIR})

file(GLOB VERTEX_SHADERS ${SHADERS_DIR}/*.vert)
foreach (VERTEX_SHADER ${VERTEX_SHADERS})
    execute_process(COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${VERTEX_SHADER} -o ${VERTEX_SHADER}.spv)
endforeach ()

file(GLOB FRAGMENT_SHADERS "${SHADERS_DIR}/*.frag")
foreach (FRAGMENT_SHADER ${FRAGMENT_SHADERS})
    execute_process(COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${FRAGMENT_SHADER} -o ${FRAGMENT_SHADER}.spv)
endforeach ()