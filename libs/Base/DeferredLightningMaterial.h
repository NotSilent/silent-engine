#pragma once

#include <vulkan/vulkan.hpp>

struct DeferredLightningMaterial {
    vk::PipelineLayout layout;
    vk::Pipeline pipeline;
    vk::DescriptorSet set;
};