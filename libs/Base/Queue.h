#pragma once

#include <vulkan/vulkan_core.h>

struct Queue {
    VkQueue queue;
    uint32_t familyIndex;
};