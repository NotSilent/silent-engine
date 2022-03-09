include(FetchContent)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG c71a50deb5ddf1ea386b91e60fa2e4a26d080074
)
add_subdirectory(imgui)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 7d5a16ce714f0b5f4efa3262de22e4d948851525
)
add_subdirectory(glfw)

FetchContent_Declare(
        tinygltf
        GIT_REPOSITORY https://github.com/syoyo/tinygltf
        GIT_TAG a159945db9d97e79a30cb34e2aaa45fd28dea576
)
add_subdirectory(tinygltf)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e
)
add_subdirectory(glm)

FetchContent_Declare(
        vk-bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG 6bf383778bfadb0eb173e6f25a9a39e7e9def1cf
)
add_subdirectory(vk-bootstrap)

FetchContent_Declare(
        vma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG 98e8cebec97df8168c5f4019ae504ea536387a81
)
add_subdirectory(vma)