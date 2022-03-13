message(STATUS "Fetching simple-ini...")

FetchContent_MakeAvailable(simple-ini)

add_library(simple-ini INTERFACE)
target_include_directories(simple-ini INTERFACE ${simple-ini_SOURCE_DIR})