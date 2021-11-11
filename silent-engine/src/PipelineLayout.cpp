#include "PipelineLayout.h"
#include <DescriptorSetLayout.h>

PipelineLayout::PipelineLayout(const vkb::Device& device, uint32_t pushSize, std::shared_ptr<DescriptorSetLayout> decriptorSetLayout)
    : _device(device)
    , _descriptorSetLayout(decriptorSetLayout)
{
    VkPushConstantRange pushConstantRange {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset = 0,
        .size = pushSize,
    };

    VkDescriptorSetLayout descriptorSetLayout = _descriptorSetLayout->getLayout();
    const VkPipelineLayoutCreateInfo pipelineLayoutInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstantRange,
    };

    if (vkCreatePipelineLayout(device.device, &pipelineLayoutInfo, nullptr, &_layout) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create pipeline layout");
    }
}

PipelineLayout::~PipelineLayout()
{
    vkDestroyPipelineLayout(_device.device, _layout, nullptr);
}

VkPipelineLayout PipelineLayout::getPipelineLayout() const
{
    return _layout;
}

std::shared_ptr<DescriptorSetLayout>  PipelineLayout::getDescriptorSetLayout() const
{
    return _descriptorSetLayout;
}