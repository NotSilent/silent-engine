include(FetchContent)

# glfw
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
)

message(STATUS "Fetching glfw...")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# glm
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
)

message(STATUS "Fetching glm...")

FetchContent_MakeAvailable(glm)

# vk-bootstrap
FetchContent_Declare(
        vk-bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG 01ef3217f7ba4bfb71a017f34875cc18771e49b9
)

message(STATUS "Fetching vk-bootstrap...")

FetchContent_MakeAvailable(vk-bootstrap)

# vma
FetchContent_Declare(
        VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG 19b940e864bd3a5afb3c79e3c6788869d01a19eb
)

message(STATUS "Fetching VulkanMemoryAllocator...")

set(VMA_RECORDING_ENABLED ON)

FetchContent_MakeAvailable(VulkanMemoryAllocator)