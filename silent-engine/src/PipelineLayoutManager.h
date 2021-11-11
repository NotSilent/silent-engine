#pragma once
#include "vk-bootstrap\VkBootstrap.h"
#include <memory>
#include <vector>

class PipelineLayout;
class DescriptorSetLayoutManager;

class PipelineLayoutManager {
public:
    PipelineLayoutManager(const vkb::Device& _device, std::shared_ptr<DescriptorSetLayoutManager> descriptorSetLayoutManager);

    std::shared_ptr<PipelineLayout> getLayout(const std::vector<VkDescriptorType>& types);

    // TODO: Remove all destroys
    void destroy();

private:
    vkb::Device _device;

    std::vector<std::shared_ptr<PipelineLayout>> _layouts;
    std::shared_ptr<DescriptorSetLayoutManager> _descriptorSetLayoutManager;
};
