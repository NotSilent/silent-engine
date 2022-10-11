#include "DescriptorSetLayout.h"

DescriptorSetLayout::DescriptorSetLayout(const vkb::Device &device, const std::vector<VkDescriptorType> &types)
        : _device(device), _types(types) {
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    for (uint32_t i = 0; i < types.size(); ++i) {
        layoutBindings.push_back({
                                         .binding = i,
                                         .descriptorType = types[i],
                                         .descriptorCount = 1,
                                         .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                                         .pImmutableSamplers = nullptr,
                                 });
    }

    VkDescriptorSetLayoutCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .bindingCount = static_cast<uint32_t>(layoutBindings.size()),
            .pBindings = layoutBindings.data(),
    };

    if (vkCreateDescriptorSetLayout(device.device, &createInfo, nullptr, &_layout) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkCreateDescriptorSetLayout");
    }
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(_device.device, _layout, nullptr);
}

VkDescriptorSetLayout DescriptorSetLayout::getLayout() const {
    return _layout;
}

bool DescriptorSetLayout::isCompatible(const std::vector<VkDescriptorType> &types) const {
    if (_types.size() != types.size()) {
        return false;
    }

    for (uint32_t i = 0; i < types.size(); ++i) {
        if (_types[i] != types[i]) {
            return false;
        }
    }

    return true;
}

const std::vector<VkDescriptorType> &DescriptorSetLayout::getDescriptorTypes() const {
    return _types;
}
