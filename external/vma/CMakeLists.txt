message(STATUS "Fetching vma...")

set(VMA_RECORDING_ENABLED ON)

FetchContent_MakeAvailable(vma)

add_library(vma INTERFACE)
target_include_directories(vma INTERFACE ${vma_SOURCE_DIR}/src)