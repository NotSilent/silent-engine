#pragma once

#include "vulkan/vulkan_core.h"

struct ShadowMapMaterial {
    VkPipelineLayout layout;
    VkPipeline pipeline;
};