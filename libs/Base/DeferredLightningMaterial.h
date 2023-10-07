#pragma once

#include <vulkan/vulkan_core.h>

struct DeferredLightningMaterial {
    VkPipelineLayout layout;
    VkPipeline pipeline;
    VkDescriptorSet set;
};