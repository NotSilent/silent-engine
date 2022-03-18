set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sengine main.cpp)

target_link_libraries(sengine base)
set_target_properties(sengine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# TODO: compile shaders at runtime
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/build/shaders/)
find_package(Vulkan)
add_custom_target(vert COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${PROJECT_SOURCE_DIR}/shaders/unlit.vert -o ${CMAKE_BINARY_DIR}/build/shaders/unlit.vert.spv)
add_custom_target(frag COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${PROJECT_SOURCE_DIR}/shaders/unlit.frag -o ${CMAKE_BINARY_DIR}/build/shaders/unlit.frag.spv)
add_dependencies(sengine vert frag)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/build/sengine.ini)
    file(WRITE ${CMAKE_BINARY_DIR}/build/sengine.ini)
endif ()