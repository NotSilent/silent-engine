#pragma once

#include "vulkan/vulkan.hpp"

struct ShadowMapMaterial {
    vk::PipelineLayout layout;
    vk::Pipeline pipeline;
};