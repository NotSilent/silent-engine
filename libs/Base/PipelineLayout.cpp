#include "PipelineLayout.h"

#include <utility>

PipelineLayout::PipelineLayout(VkDevice device,
                               uint32_t pushSize)
        : device(device) {
    VkPushConstantRange pushConstantRange{
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = pushSize,
    };

    const VkPipelineLayoutCreateInfo pipelineLayoutInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &pushConstantRange,
    };

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &_layout) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create pipeline layout");
    }
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(device, _layout, nullptr);
}

VkPipelineLayout PipelineLayout::getPipelineLayout() const {
    return _layout;
}