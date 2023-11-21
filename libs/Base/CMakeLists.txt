set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BASE_SOURCES
        Buffer.h Buffer.cpp
        Camera.h Camera.cpp
        DrawData.h DrawData.cpp
        Image.h Image.cpp
        Implementations.cpp
        Mesh.h Mesh.cpp
        ProfillerTimer.h ProfillerTimer.cpp
        PushData.h
        Renderer.h Renderer.cpp
        VkInit.h VkInit.cpp
        Window.h Window.cpp

        CameraComponent.h CameraComponent.cpp
        Component.h Component.cpp
        Entity.h Entity.cpp
        MeshComponent.h MeshComponent.cpp

        BufferManager.h BufferManager.cpp
        ImageManager.h ImageManager.cpp
        PipelineManager.h PipelineManager.cpp

        EngineStatics.h EngineStatics.cpp
        InputSystem.h InputSystem.cpp
        TimeSystem.h TimeSystem.cpp
        CommandBuffer.cpp CommandBuffer.h
        stb_image.h
        FrameResources.cpp FrameResources.h
        PushData.cpp
        Shader.h
        ShaderManager.cpp
        ShaderManager.h
        DeferredRenderpass.cpp
        DeferredRenderpass.h
        PatchedSphere.cpp
        PatchedSphere.h
        DeferredLightningRenderpass.cpp
        DeferredLightningRenderpass.h
        DeferredLightningMaterial.cpp
        DeferredLightningMaterial.h
        ShadowMapRenderpass.cpp
        ShadowMapRenderpass.h
        DirectionalLight.cpp
        DirectionalLight.h
        ShadowMapMaterial.h
)

add_library(base STATIC ${BASE_SOURCES})

find_package(Vulkan COMPONENTS glslc glslang shaderc_combined SPIRV-Tools)
target_link_libraries(base Vulkan::Vulkan)
target_link_libraries(base Vulkan::glslang)
target_link_libraries(base Vulkan::shaderc_combined)
target_link_libraries(base SPIRV-Tools-opt)
target_link_libraries(base SPIRV-Tools)

target_compile_definitions(glm INTERFACE GLM_FORCE_LEFT_HANDED)
target_compile_definitions(glm INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE)

target_link_libraries(base glm)
target_link_libraries(base imgui)
target_link_libraries(base vk-bootstrap)
target_link_libraries(base VulkanMemoryAllocator)

target_include_directories(base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(base PUBLIC Vulkan::Headers)