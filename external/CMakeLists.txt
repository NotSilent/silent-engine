include(FetchContent)
set(FETCHCONTENT_QUIET off)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 7d5a16ce714f0b5f4efa3262de22e4d948851525
)
add_subdirectory(glfw)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e
)
add_subdirectory(glm)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG d7c8516a4b848c0291e3d75b627c0843f515f591
)
add_subdirectory(imgui)

FetchContent_Declare(
        vk-bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG 8e61b2d81c3f5f84339735085ff5651f71bbe1e7
)
add_subdirectory(vk-bootstrap)

FetchContent_Declare(
        VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG 5a8fd79c597576a139716d48f264441733ad6d3b
)
add_subdirectory(VulkanMemoryAllocator)