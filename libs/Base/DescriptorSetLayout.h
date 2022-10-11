#pragma once

#include <vector>
#include <vulkan\vulkan.h>
#include "VkBootstrap.h"

class DescriptorSetLayout {
public:
    DescriptorSetLayout(const vkb::Device &device, const std::vector<VkDescriptorType> &types);

    ~DescriptorSetLayout();

    [[nodiscard]] VkDescriptorSetLayout getLayout() const;

    [[nodiscard]] const std::vector<VkDescriptorType> &getDescriptorTypes() const;

    [[nodiscard]] bool isCompatible(const std::vector<VkDescriptorType> &types) const;

private:
    vkb::Device _device;

    VkDescriptorSetLayout _layout;
    std::vector<VkDescriptorType> _types;
};
