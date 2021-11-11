#pragma once
#include <memory>
#include <vk-bootstrap\VkBootstrap.h>

class DescriptorSetLayout;

class PipelineLayout {
public:
    PipelineLayout(const vkb::Device& device, uint32_t pushSize, std::shared_ptr<DescriptorSetLayout> decriptorSetLayout);
    ~PipelineLayout();

    VkPipelineLayout getPipelineLayout() const;
    std::shared_ptr<DescriptorSetLayout> getDescriptorSetLayout() const;

private:
    vkb::Device _device;

    VkPipelineLayout _layout;

    std::shared_ptr<DescriptorSetLayout> _descriptorSetLayout;
};
