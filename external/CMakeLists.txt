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
        nativefiledialog-extended
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended
        GIT_TAG 6efc824070c79afd42a3a9e08da8f867d0ca2a97
)
add_subdirectory(nativefiledialog-extended)

FetchContent_Declare(
        simple-ini
        GIT_REPOSITORY https://github.com/brofield/simpleini
        GIT_TAG 9b3ed7ec815997bc8c5b9edf140d6bde653e1458
)
add_subdirectory(simple-ini)

FetchContent_Declare(
        vk-bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG 8e61b2d81c3f5f84339735085ff5651f71bbe1e7
)
add_subdirectory(vk-bootstrap)

FetchContent_Declare(
        VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG a6bfc237255a6bac1513f7c1ebde6d8aed6b5191
)
add_subdirectory(VulkanMemoryAllocator)