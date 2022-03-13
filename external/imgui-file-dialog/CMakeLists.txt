message(STATUS "Fetching imgui-file-dialog...")

FetchContent_MakeAvailable(imgui-file-dialog)

set(imgui-file-dialog_SOURCES
        ${imgui-file-dialog_SOURCE_DIR}/ImGuiFileDialog.h
        ${imgui-file-dialog_SOURCE_DIR}/ImGuiFileDialogConfig.h
        ${imgui-file-dialog_SOURCE_DIR}/ImGuiFileDialog.cpp)

add_library(imgui-file-dialog STATIC ${imgui-file-dialog_SOURCES})
target_compile_definitions(imgui-file-dialog PRIVATE -DUSE_BOOKMARK)
target_include_directories(imgui-file-dialog PUBLIC ${imgui-file-dialog_SOURCE_DIR})
target_link_libraries(imgui-file-dialog imgui)