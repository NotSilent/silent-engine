set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BASE_SOURCES
        Buffer.h Buffer.cpp
        Camera.h Camera.cpp
        DescriptorSet.h DescriptorSet.cpp
        DescriptorSetLayout.h DescriptorSetLayout.cpp
        DrawData.h DrawData.cpp
        Image.h Image.cpp
        Implementations.cpp
        Material.h Material.cpp
        Mesh.h Mesh.cpp
        Pipeline.h Pipeline.cpp
        PipelineLayout.h PipelineLayout.cpp
        ProfillerTimer.h ProfillerTimer.cpp
        PushData.h
        Renderer.h Renderer.cpp
        Sampler.h Sampler.cpp
        Texture.h Texture.cpp
        Vertex.h
        VertexAttribute.h VertexAttribute.cpp
        VkDraw.h VkDraw.cpp
        VkInit.h VkInit.cpp
        Window.h Window.cpp

        CameraComponent.h CameraComponent.cpp
        Component.h Component.cpp
        Entity.h Entity.cpp
        MeshComponent.h MeshComponent.cpp

        BufferManager.h BufferManager.cpp
        DescriptorSetLayoutManager.h DescriptorSetLayoutManager.cpp
        DescriptorSetManager.h DescriptorSetManager.cpp
        ImageManager.h ImageManager.cpp
        MaterialManager.h MaterialManager.cpp
        PipelineLayoutManager.h PipelineLayoutManager.cpp
        PipelineManager.h PipelineManager.cpp
        SamplerManager.h SamplerManager.cpp
        TextureManager.h TextureManager.cpp

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
)

add_library(base STATIC ${BASE_SOURCES})

find_package(Vulkan COMPONENTS glslc glslang shaderc_combined SPIRV-Tools)
target_link_libraries(base Vulkan::Vulkan)
target_link_libraries(base Vulkan::glslang)
target_link_libraries(base Vulkan::shaderc_combined)
target_link_libraries(base SPIRV-Tools-opt)
target_link_libraries(base SPIRV-Tools)

target_link_libraries(base glm)
target_link_libraries(base imgui)
target_link_libraries(base vk-bootstrap)
target_link_libraries(base VulkanMemoryAllocator)

target_include_directories(base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(base PUBLIC Vulkan::Headers)