#include "PipelineManager.h"
#include <algorithm>
#include <PushData.h>
#include <array>

PipelineManager::PipelineManager(VkDevice device, VkFormat swapchainFormat, const VkRect2D& renderArea)
        : device(device)
        , renderArea(renderArea)
        , shaderManager(device){

    VkPushConstantRange deferredPushConstantRange{
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(PushData),
    };

    descriptorPool = createDescriptorPool();
    compositeDescriptorSetLayout = createDescriptorSetLayout();
    compositeSets[0] = createCompositeSet();
    compositeSets[1] = createCompositeSet();
    compositeSets[2] = createCompositeSet();
    deferredPipelineLayout = createPipelineLayout(0, nullptr, 1, &deferredPushConstantRange);
    compositePipelineLayout = createPipelineLayout(1, &compositeDescriptorSetLayout, 0, nullptr);
    deferredPipeline = createDeferredPipeline();
    compositePipeline = createCompositePipeline(swapchainFormat);
}

void PipelineManager::destroy() {
    vkDestroyPipeline(device, deferredPipeline, nullptr);
    vkDestroyPipeline(device, compositePipeline, nullptr);
    vkDestroyPipelineLayout(device, deferredPipelineLayout, nullptr);
    vkDestroyPipelineLayout(device, compositePipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, compositeDescriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);

    shaderManager.destroy();
}

VkDescriptorSet PipelineManager::getCompositeSet(uint32_t frameIndex) const {
    return compositeSets[frameIndex];
}

VkPipelineLayout PipelineManager::getDeferredPipelineLayout() const {
    return deferredPipelineLayout;
}

VkPipelineLayout PipelineManager::getCompositePipelineLayout() const {
    return compositePipelineLayout;
}

VkPipeline PipelineManager::getDeferredPipeline() const {
    return deferredPipeline;
}

VkPipeline PipelineManager::getCompositePipeline() const {
    return compositePipeline;
}

VkDescriptorPool PipelineManager::createDescriptorPool() {
    // TODO: Configurable and per type
    static uint32_t DescriptorSetCount = 3;

    std::array descriptorPoolSizes {
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = DescriptorSetCount,
        }
    };

    VkDescriptorPoolCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .maxSets = DescriptorSetCount,
        .poolSizeCount = descriptorPoolSizes.size(),
        .pPoolSizes = descriptorPoolSizes.data(),
    };

    VkDescriptorPool descriptorPool;
    if(vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("PipelineManager::createDescriptorPool");
    }

    return descriptorPool;
}

VkDescriptorSetLayout PipelineManager::createDescriptorSetLayout() {
    // TODO: try immutable samplers for deferred lightning
    std::array bindings {
            VkDescriptorSetLayoutBinding {
                    .binding = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .pImmutableSamplers = nullptr,
            }
    };

    VkDescriptorSetLayoutCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .bindingCount = bindings.size(),
            .pBindings = bindings.data(),
    };

    VkDescriptorSetLayout descriptorSetLayout;
    if(vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("PipelineManager::vkCreateDescriptorSetLayout");
    }

    return descriptorSetLayout;
}

VkDescriptorSet PipelineManager::createCompositeSet() {
    VkDescriptorSetAllocateInfo allocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &compositeDescriptorSetLayout,
    };

    VkDescriptorSet descriptorSet;
    if(vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet)) {
        throw std::runtime_error("PipelineManager::createCompositeSet");
    }

    return descriptorSet;
}

VkPipelineLayout PipelineManager::createPipelineLayout(uint32_t setLayoutCount, const VkDescriptorSetLayout* pSetLayouts,
                                                       uint32_t pushConstantRangeCount, const VkPushConstantRange* pushConstantRange) {
    const VkPipelineLayoutCreateInfo pipelineLayoutInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .setLayoutCount = setLayoutCount,
            .pSetLayouts = pSetLayouts,
            .pushConstantRangeCount = pushConstantRangeCount,
            .pPushConstantRanges = pushConstantRange,
    };

    VkPipelineLayout pipeline;
    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline) != VK_SUCCESS) {
        throw std::runtime_error("PipelineManager::createPipelineLayout");
    }

    return pipeline;
}

VkPipeline PipelineManager::createDeferredPipeline() {
    std::optional<Shader> shader = shaderManager.getShader("pbrDeferred");
    if(shader.has_value())
    {
        std::array<VkVertexInputBindingDescription, 1> vertexBindingDescriptions = {
                VkVertexInputBindingDescription {
                        .binding = 0,
                        .stride = sizeof(glm::vec3),
                        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                }
        };

        std::array<VkVertexInputAttributeDescription, 1> vertexInputAttributeDescription {
                VkVertexInputAttributeDescription {
                        .location = 0,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset = 0,
                }
        };

        std::array<VkPipelineColorBlendAttachmentState, 1> colorBlendAttachments {
                createPipelineColorBlendAttachmentState(),
        };

        // TODO: manage format
        std::array<VkFormat, 1> colorAttachmentFormats {VK_FORMAT_R8G8B8A8_UNORM};

        return createPipeline(shader.value(),
                              vertexBindingDescriptions.size(),
                              vertexBindingDescriptions.data(),
                              vertexInputAttributeDescription.size(),
                              vertexInputAttributeDescription.data(),
                              colorBlendAttachments.size(),
                              colorBlendAttachments.data(),
                              colorAttachmentFormats.size(),
                              colorAttachmentFormats.data(),
                              deferredPipelineLayout,
                              VK_CULL_MODE_BACK_BIT);
    }

    return {};
}

VkPipeline PipelineManager::createCompositePipeline(VkFormat swapchainFormat) {
    std::optional<Shader> shader = shaderManager.getShader("pbrComposite");
    if(shader.has_value())
    {
        std::array<VkPipelineColorBlendAttachmentState, 1> colorBlendAttachments {
                createPipelineColorBlendAttachmentState(),
        };

        std::array<VkFormat, 1> colorAttachmentFormats {swapchainFormat};

        return createPipeline(shader.value(),
                              0,
                              nullptr,
                              0,
                              nullptr,
                              colorBlendAttachments.size(),
                              colorBlendAttachments.data(),
                              colorAttachmentFormats.size(),
                              colorAttachmentFormats.data(),
                              compositePipelineLayout,
                              VK_CULL_MODE_NONE);
    }

    return {};
}

VkPipeline PipelineManager::createPipeline(const Shader& shader,
                                                          uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription* pVertexBindingDescriptions,
                                                          uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription* pVertexAttributeDescriptions,
                                                          uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* pAttachments,
                                                          uint32_t colorAttachmentCount, const VkFormat* pColorAttachmentFormats,
                                                          VkPipelineLayout pipelineLayout, VkCullModeFlags cullMode) {
    const std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageCreateInfos = {
            createPipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shader.vert),
            createPipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, shader.frag),
    };

    const VkPipelineVertexInputStateCreateInfo vertexInputState = createPipelineVertexInputStateCreateInfo(
            vertexBindingDescriptionCount, pVertexBindingDescriptions, vertexAttributeDescriptionCount, pVertexAttributeDescriptions);

    const VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = createPipelineInputAssemblyStateCreateInfo();
    const VkPipelineTessellationStateCreateInfo tessellationState = createPipelineTessellationStateCreateInfo();

    const VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(renderArea.extent.width),
            .height = static_cast<float>(renderArea.extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };

    const VkPipelineViewportStateCreateInfo viewportState = createViewportStateCreateInfo(viewport, renderArea);
    const VkPipelineRasterizationStateCreateInfo rasterizationState = createRasterizationStateCreateInfo(cullMode);
    const VkPipelineMultisampleStateCreateInfo multisampleState = createPipelineMultisampleStateCreateInfo();
    const VkPipelineDepthStencilStateCreateInfo depthStencilState = createPipelineDepthStencilStateCreateInfo();
    const VkPipelineColorBlendStateCreateInfo colorBlendState = createPipelineColorBlendStateCreateInfo(attachmentCount, pAttachments);
    const VkPipelineDynamicStateCreateInfo dynamicState = createPipelineDynamicStateCreateInfo();
    const VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = createPipelineRenderingCreateInfoKHR(colorAttachmentCount, pColorAttachmentFormats);

    VkGraphicsPipelineCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &pipelineRenderingCreateInfo,
            .flags = {},
            .stageCount = shaderStageCreateInfos.size(),
            .pStages = shaderStageCreateInfos.data(),
            .pVertexInputState = &vertexInputState,
            .pInputAssemblyState = &inputAssemblyState,
            .pTessellationState = &tessellationState,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizationState,
            .pMultisampleState = &multisampleState,
            .pDepthStencilState = &depthStencilState,
            .pColorBlendState = &colorBlendState,
            .pDynamicState = &dynamicState,
            .layout = pipelineLayout,
            .renderPass = nullptr,
            .subpass = 0,
            .basePipelineHandle = nullptr,
            .basePipelineIndex = 0,
    };

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(device, nullptr, 1, &createInfo, nullptr, &pipeline) != VK_SUCCESS) {
        throw std::runtime_error("PipelineManager::createPipeline");
    }

    return pipeline;
}

VkPipelineShaderStageCreateInfo
PipelineManager::createPipelineShaderStageCreateInfo(VkShaderStageFlagBits shaderStage, VkShaderModule module) {
    return {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .stage = shaderStage,
            .module = module,
            .pName = "main",
            .pSpecializationInfo = nullptr,
    };
}

VkPipelineVertexInputStateCreateInfo
PipelineManager::createPipelineVertexInputStateCreateInfo(uint32_t vertexBindingDescriptionCount,
                                                          const VkVertexInputBindingDescription *pVertexBindingDescriptions,
                                                          uint32_t vertexAttributeDescriptionCount,
                                                          const VkVertexInputAttributeDescription *pVertexAttributeDescriptions) {
    return VkPipelineVertexInputStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .vertexBindingDescriptionCount = vertexBindingDescriptionCount,
            .pVertexBindingDescriptions = pVertexBindingDescriptions,
            .vertexAttributeDescriptionCount = vertexAttributeDescriptionCount,
            .pVertexAttributeDescriptions = pVertexAttributeDescriptions,
    };
}

VkPipelineRasterizationStateCreateInfo
PipelineManager::createRasterizationStateCreateInfo(VkCullModeFlags cullMode) {
    return VkPipelineRasterizationStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .depthClampEnable = false,
            .rasterizerDiscardEnable = false,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = cullMode,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = false,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f,
    };
}

VkPipelineInputAssemblyStateCreateInfo PipelineManager::createPipelineInputAssemblyStateCreateInfo() {
    return VkPipelineInputAssemblyStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = false,
    };
}

VkPipelineTessellationStateCreateInfo PipelineManager::createPipelineTessellationStateCreateInfo() {
    return VkPipelineTessellationStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .patchControlPoints = 0,
    };
}

VkPipelineViewportStateCreateInfo PipelineManager::createViewportStateCreateInfo(const VkViewport& viewport, const VkRect2D& renderArea) {
    return VkPipelineViewportStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &renderArea,
    };
}

VkPipelineMultisampleStateCreateInfo PipelineManager::createPipelineMultisampleStateCreateInfo() {
    return VkPipelineMultisampleStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 0.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
    };
}

VkPipelineDepthStencilStateCreateInfo PipelineManager::createPipelineDepthStencilStateCreateInfo() {
    return VkPipelineDepthStencilStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .depthTestEnable = true,
            .depthWriteEnable = true,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = true,
            .stencilTestEnable = true,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f,
    };
}

VkPipelineColorBlendAttachmentState PipelineManager::createPipelineColorBlendAttachmentState() {
    return VkPipelineColorBlendAttachmentState {
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = {},
            .dstColorBlendFactor = {},
            .colorBlendOp = {},
            .srcAlphaBlendFactor = {},
            .dstAlphaBlendFactor = {},
            .alphaBlendOp = {},
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                              VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT |
                              VK_COLOR_COMPONENT_A_BIT,
    };
}

VkPipelineColorBlendStateCreateInfo PipelineManager::createPipelineColorBlendStateCreateInfo(uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* pAttachments) {
    return VkPipelineColorBlendStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .logicOpEnable = false,
            .logicOp = {},
            .attachmentCount = attachmentCount,
            .pAttachments = pAttachments,
            .blendConstants = {},
    };
}

VkPipelineDynamicStateCreateInfo PipelineManager::createPipelineDynamicStateCreateInfo() {
    return VkPipelineDynamicStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .dynamicStateCount = 0,
            .pDynamicStates = nullptr,
    };
}

VkPipelineRenderingCreateInfoKHR PipelineManager::createPipelineRenderingCreateInfoKHR(uint32_t colorAttachmentCount, const VkFormat* pColorAttachmentFormats) {
    return VkPipelineRenderingCreateInfoKHR {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
            .pNext = nullptr,
            .viewMask = 0,
            .colorAttachmentCount = colorAttachmentCount,
            .pColorAttachmentFormats = pColorAttachmentFormats,
            .depthAttachmentFormat = VK_FORMAT_D32_SFLOAT,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };
}