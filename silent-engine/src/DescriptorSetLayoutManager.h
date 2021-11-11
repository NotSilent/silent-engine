#pragma once
#include "vk-bootstrap\VkBootstrap.h"
#include <memory>

class DescriptorSetLayout;

class DescriptorSetLayoutManager {
public:
    DescriptorSetLayoutManager(const vkb::Device& device);

    std::shared_ptr<DescriptorSetLayout> getLayout(const std::vector<VkDescriptorType>& types);

    // TODO: Remove all destroys
    void destroy();

private:
    vkb::Device _device;

    std::vector<std::shared_ptr<DescriptorSetLayout>> _layouts;
};
