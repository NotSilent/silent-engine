#include "Pipeline.h"
#include "PipelineLayout.h"
#include <fstream>
#include <array>

#include "glm/glm.hpp"

Pipeline::Pipeline(VkDevice device, float width, float height,
                   const std::shared_ptr<PipelineLayout>& layout, VkFormat swapchainFormat, const Shader &shader)
        : device(device)
        , layout(layout) {
    static const uint32_t SHADER_STAGES = 2;

    const VkPipelineShaderStageCreateInfo shaderStageCreateInfos[SHADER_STAGES]{
            createPipelineShaderStageCreateinfo(VK_SHADER_STAGE_VERTEX_BIT, shader.vert),
            createPipelineShaderStageCreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, shader.frag),
    };

    std::array vertexInputBindingDescriptions {
        VkVertexInputBindingDescription {
            .binding = 0,
            .stride = sizeof(glm::vec3),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        }
    };
    std::array vertexInputAttributeDescriptions {
            VkVertexInputAttributeDescription {
                    .location = 0,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32B32_SFLOAT,
                    .offset = 0,
            }
    };

    const VkPipelineVertexInputStateCreateInfo vertexInputState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = vertexInputBindingDescriptions.size(),
            .pVertexBindingDescriptions = vertexInputBindingDescriptions.data(),
            .vertexAttributeDescriptionCount = vertexInputAttributeDescriptions.size(),
            .pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data(),
    };

    const VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
    };

    const VkPipelineTessellationStateCreateInfo tessellationState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .patchControlPoints = 0,
    };

    const VkViewport viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = width,
            .height = height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };

    const VkRect2D scissor{{0,                            0},
                           {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}};

    const VkPipelineViewportStateCreateInfo viewportState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor,
    };

    const VkPipelineRasterizationStateCreateInfo rasterizationState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f,
    };

    const VkPipelineMultisampleStateCreateInfo multisampleState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 0.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
    };

    const VkPipelineDepthStencilStateCreateInfo depthStencilState{
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

    // TODO: Create for renderpass?
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    colorBlendAttachments.push_back({
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
                                    });
    colorBlendAttachments.push_back({
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
                                    });

    const VkPipelineColorBlendStateCreateInfo colorBlendState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = false,
            .logicOp = {},
            .attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size()),
            .pAttachments = colorBlendAttachments.data(),
            .blendConstants = {},
    };

    const VkPipelineDynamicStateCreateInfo dynamicState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = 0,
            .pDynamicStates = nullptr,
    };

    // TODO: When per renderpass pipelines created, somehow control the format
    std::array colorAttachmentFormats {VK_FORMAT_R8G8B8A8_UNORM, swapchainFormat};
    const VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
            .viewMask = 0,
            .colorAttachmentCount = colorAttachmentFormats.size(),
            .pColorAttachmentFormats = colorAttachmentFormats.data(),
            .depthAttachmentFormat = VK_FORMAT_D32_SFLOAT,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    VkGraphicsPipelineCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &pipelineRenderingCreateInfo,
            .stageCount = SHADER_STAGES,
            .pStages = shaderStageCreateInfos,
            .pVertexInputState = &vertexInputState,
            .pInputAssemblyState = &inputAssemblyState,
            .pTessellationState = &tessellationState,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizationState,
            .pMultisampleState = &multisampleState,
            .pDepthStencilState = &depthStencilState,
            .pColorBlendState = &colorBlendState,
            .pDynamicState = &dynamicState,
            .layout = layout->getPipelineLayout(),
            .renderPass = nullptr,
            .subpass = 0,
            .basePipelineHandle = nullptr,
            .basePipelineIndex = 0,
    };

    if (vkCreateGraphicsPipelines(device, nullptr, 1, &createInfo, nullptr, &pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create pipeline");
    }
}

Pipeline::~Pipeline() {
    vkDestroyPipeline(device, pipeline, nullptr);
}

VkPipeline Pipeline::getPipeline() const {
    return pipeline;
}

VkPipelineLayout Pipeline::getPipelineLayout() const {
    return layout->getPipelineLayout();
}

bool Pipeline::isCompatible(const std::shared_ptr<PipelineLayout> &otherLayout) {
    if (layout != otherLayout) {
        return false;
    }

    return true;
}

VkPipelineShaderStageCreateInfo
Pipeline::createPipelineShaderStageCreateinfo(VkShaderStageFlagBits shaderStage, VkShaderModule module) {
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