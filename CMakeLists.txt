cmake_minimum_required(VERSION 3.17)
project(SilentEngine)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
add_subdirectory(tinygltf)

add_subdirectory(glm)

add_subdirectory(vk-bootstrap)

set(VMA_RECORDING_ENABLED ON)
add_subdirectory(VulkanMemoryAllocator)

add_subdirectory(SilentEngine)

set(IMGUI_DIR imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
add_library(ImGui ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)
target_include_directories(ImGui PUBLIC ${IMGUI_DIR})
find_package(Vulkan REQUIRED)
target_link_libraries(ImGui glfw Vulkan::Vulkan)
# Compile as static library
# file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)
# add_library(ImGui STATIC ${IMGUI_SOURCES})
# target_include_directories(ImGui PUBLIC ${IMGUI_PATH})