#pragma once

#include "VkBootstrap.h"
#include <memory>
#include <vulkan\vulkan.h>
#include <vector>

class Texture;

class DescriptorSetLayout;

class DescriptorSet {
public:
    DescriptorSet(const vkb::Device &device, VkDescriptorPool descriptorPool,
                  std::shared_ptr<DescriptorSetLayout> layout, const std::vector<std::shared_ptr<Texture>> &textures);

    ~DescriptorSet();

    [[nodiscard]] VkDescriptorSet getDescriptorSet() const;

    bool
    isCompatible(std::shared_ptr<DescriptorSetLayout> layout, const std::vector<std::shared_ptr<Texture>> &textures);

private:
    vkb::Device _device;

    VkDescriptorSet _descriptorSet;
    std::shared_ptr<DescriptorSetLayout> _layout;
    std::vector<std::shared_ptr<Texture>> _textures;
};
