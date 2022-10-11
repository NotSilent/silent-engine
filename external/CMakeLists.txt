include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 7d5a16ce714f0b5f4efa3262de22e4d948851525
        FETCHCONTENT_UPDATES_DISCONNECTED
)
add_subdirectory(glfw)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e
        FETCHCONTENT_UPDATES_DISCONNECTED
)
add_subdirectory(glm)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG c71a50deb5ddf1ea386b91e60fa2e4a26d080074
        FETCHCONTENT_UPDATES_DISCONNECTED
)
add_subdirectory(imgui)

FetchContent_Declare(
        nativefiledialog-extended
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended
        GIT_TAG 28ade5a5cc5d17cea8fe4034572cac8fd54eb53f
        FETCHCONTENT_UPDATES_DISCONNECTED
)
add_subdirectory(nativefiledialog-extended)

FetchContent_Declare(
        simple-ini
        GIT_REPOSITORY https://github.com/brofield/simpleini
        GIT_TAG cfb8820b4fa133b5bdc8c2f423be18bf4873687b
        FETCHCONTENT_UPDATES_DISCONNECTED
)
add_subdirectory(simple-ini)

FetchContent_Declare(
        tinygltf
        GIT_REPOSITORY https://github.com/syoyo/tinygltf
        GIT_TAG 1a7c7d20013465b5c2a1285872b52a43ab62c1b3
        FETCHCONTENT_UPDATES_DISCONNECTED
)
add_subdirectory(tinygltf)

FetchContent_Declare(
        vk-bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG 6bf383778bfadb0eb173e6f25a9a39e7e9def1cf
        FETCHCONTENT_UPDATES_DISCONNECTED
)
add_subdirectory(vk-bootstrap)

FetchContent_Declare(
        VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG 05a61447182a00e479b361e4e363d5d9b54b818a
        FETCHCONTENT_UPDATES_DISCONNECTED
)
add_subdirectory(VulkanMemoryAllocator)