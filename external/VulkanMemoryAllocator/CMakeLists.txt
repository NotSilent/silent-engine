message(STATUS "Fetching VulkanMemoryAllocator...")

set(VMA_RECORDING_ENABLED ON)

FetchContent_MakeAvailable(VulkanMemoryAllocator)