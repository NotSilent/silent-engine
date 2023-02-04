#include "Pipeline.h"
#include "PipelineLayout.h"
#include "VertexAttribute.h"
#include <fstream>

Pipeline::Pipeline(const vkb::Device &device, float width, float height,
                   const std::vector<VertexAttributeDescription> &attributeDescriptions,
                   std::shared_ptr<PipelineLayout> layout, const std::string &shaderName)
        : _device(device), _shaderName(shaderName),
          _attributeDescriptions(attributeDescriptions), _layout(layout) {
    static const uint32_t SHADER_STAGES = 2;

    const auto vertexModule = createShaderModule(device, shaderName + ".vert.spv");
    const auto fragmentModule = createShaderModule(device, shaderName + ".frag.spv");

    const VkPipelineShaderStageCreateInfo shaderStageCreateInfos[SHADER_STAGES]{
            createPipelineShaderStageCreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexModule),
            createPipelineShaderStageCreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentModule),
    };

    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
    for (uint32_t i = 0; i < attributeDescriptions.size(); ++i) {
        vertexInputBindingDescriptions.push_back({
                                                         .binding = i,
                                                         .stride = attributeDescriptions[i].stride,
                                                         .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                                                 });

        vertexInputAttributeDescriptions.push_back({
                                                           .location = i,
                                                           .binding = i,
                                                           .format = attributeDescriptions[i].format,
                                                           .offset = 0,
                                                   });
    }

    const VkPipelineVertexInputStateCreateInfo vertexInputState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescriptions.size()),
            .pVertexBindingDescriptions = vertexInputBindingDescriptions.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size()),
            .pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data(),
    };

    const VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
    };

    const VkPipelineTessellationStateCreateInfo tessellationState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
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
            .pNext = nullptr,
            .flags = {},
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor,
    };

    const VkPipelineRasterizationStateCreateInfo rasterizationState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
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
            .pNext = nullptr,
            .flags = {},
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
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f,
    };

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    colorBlendAttachments.push_back({
                                            .blendEnable = VK_FALSE,
                                            .srcColorBlendFactor = {},
                                            .dstColorBlendFactor = {},
                                            .colorBlendOp = {},
                                            .srcAlphaBlendFactor = {},
                                            .dstAlphaBlendFactor = {},
                                            .alphaBlendOp = {},
                                            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                              VK_COLOR_COMPONENT_B_BIT |
                                                              VK_COLOR_COMPONENT_A_BIT,
                                    });

    const VkPipelineColorBlendStateCreateInfo colorBlendState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .logicOpEnable = VK_FALSE,
            .logicOp = {},
            .attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size()),
            .pAttachments = colorBlendAttachments.data(),
            .blendConstants = {},
    };

    const VkPipelineDynamicStateCreateInfo dynamicState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .dynamicStateCount = 0,
            .pDynamicStates = nullptr,
    };

    VkGraphicsPipelineCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
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
            .layout = _layout->getPipelineLayout(),
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
    };

    if (vkCreateGraphicsPipelines(device.device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create pipeline");
    }

    vkDestroyShaderModule(device.device, vertexModule, nullptr);
    vkDestroyShaderModule(device.device, fragmentModule, nullptr);
}

Pipeline::~Pipeline() {
    vkDestroyPipeline(_device.device, _pipeline, nullptr);
}

VkPipeline Pipeline::getPipeline() const {
    return _pipeline;
}

VkPipelineLayout Pipeline::getPipelineLayout() const {
    return _layout->getPipelineLayout();
}

bool Pipeline::isCompatible(const std::vector<VertexAttributeDescription> &attributeDescriptions,
                            const std::shared_ptr<PipelineLayout> &layout, const std::string &shaderName) {
    if (_layout != layout) {
        return false;
    }

    if (_shaderName != shaderName) {
        return false;
    }

    if (_attributeDescriptions.size() != attributeDescriptions.size()) {
        return false;
    }

    for (uint32_t i = 0; i < attributeDescriptions.size(); ++i) {
        if (_attributeDescriptions[i] != attributeDescriptions[i]) {
            return false;
        }
    }

    return true;
}

std::tuple<size_t, std::vector<char>> Pipeline::getShaderDataFromFile(const std::string &shaderPath) {
    std::ifstream file(shaderPath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't find file:" + shaderPath);
    }

    const auto fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    return {fileSize, buffer};
}

VkShaderModule Pipeline::createShaderModule(const vkb::Device &device, const std::string &shaderFilename) {
    const auto shaderData = getShaderDataFromFile("shaders/" + shaderFilename);

    const VkShaderModuleCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .codeSize = std::get<0>(shaderData),
            .pCode = reinterpret_cast<const uint32_t *>(std::get<1>(shaderData).data()),
    };

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create shader module");
    }

    return shaderModule;
}

VkPipelineShaderStageCreateInfo
Pipeline::createPipelineShaderStageCreateinfo(VkShaderStageFlagBits shaderStage, VkShaderModule

module) {
return {
.
sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
.
pNext = nullptr,
.
flags = {},
.
stage = shaderStage,
.module = module,
.
pName = "main",
.
pSpecializationInfo = nullptr,
};
}