#pragma once
#include "vk-bootstrap\VkBootstrap.h"
#include <memory>
#include <vector>

class DescriptorSet;
class DescriptorSetLayoutManager;
class Texture;

class DescriptorSetManager {
public:
    DescriptorSetManager(const vkb::Device& device, std::shared_ptr<DescriptorSetLayoutManager> descriptorSetLayoutManager);

    std::shared_ptr<DescriptorSet> getDescriptorSet(const std::vector<VkDescriptorType> types, std::vector<std::shared_ptr<Texture>>& textures);

    // TODO: Remove all destroys
    void destroy();

private:
    vkb::Device _device;

    VkDescriptorPool _descriptorPool;

    std::vector<std::shared_ptr<DescriptorSet>> _descriptorSets;
    std::shared_ptr<DescriptorSetLayoutManager> _descriptorSetLayoutManager;
};
