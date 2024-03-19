#pragma once

#include <vulkan/vulkan.hpp>

struct Queue {
    vk::Queue queue;
    uint32_t familyIndex = vk::QueueFamilyIgnored;
};