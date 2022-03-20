set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BASE_SOURCES
        Buffer.h
        Camera.h Camera.cpp
        DescriptorSet.h DescriptorSet.cpp
        DescriptorSetLayout.h DescriptorSetLayout.cpp
        DrawData.h DrawData.cpp
        Image.h
        ImGuiData.h
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
        VkResource.h
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
        )

add_library(base STATIC ${BASE_SOURCES})

find_package(Vulkan)
target_link_libraries(base Vulkan::Vulkan)

target_link_libraries(base glm)
target_link_libraries(base imgui)
target_link_libraries(base imgui-file-dialog)
target_link_libraries(base simple-ini)
target_link_libraries(base tinygltf)
target_link_libraries(base vk-bootstrap)
target_link_libraries(base VulkanMemoryAllocator)

target_include_directories(base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})