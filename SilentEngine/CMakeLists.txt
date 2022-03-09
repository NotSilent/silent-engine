set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SILENT_ENGINE_FILES CONFIGURE_DEPENDS "*.h" "*.cpp")
file(GLOB SILENT_ENGINE_FILES2 CONFIGURE_DEPENDS "ECS/*.h" "ECS/*.cpp")
file(GLOB SILENT_ENGINE_FILES3 CONFIGURE_DEPENDS "EngineManagers/*.h" "EngineManagers/*.cpp")
file(GLOB SILENT_ENGINE_FILES4 CONFIGURE_DEPENDS "EngineSystems/*.h" "EngineSystems/*.cpp")

add_executable(SilentEngine ${SILENT_ENGINE_FILES} ${SILENT_ENGINE_FILES2} ${SILENT_ENGINE_FILES3} ${SILENT_ENGINE_FILES4})

find_package(Vulkan REQUIRED)
target_link_libraries(SilentEngine Vulkan::Vulkan)

target_link_libraries(SilentEngine glfw)
target_link_libraries(SilentEngine tinygltf)
target_link_libraries(SilentEngine glm)
target_link_libraries(SilentEngine vk-bootstrap)
target_link_libraries(SilentEngine vma)
target_link_libraries(SilentEngine imgui)