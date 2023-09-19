#pragma once

#include <vulkan/vulkan_core.h>
#include "VkBootstrap.h"
#include "vk_mem_alloc.h"

class Sampler {
public:
    Sampler() = default;

    Sampler(const vkb::Device &device);

    ~Sampler();

    VkSampler getSampler() const;

    void destroy(VkDevice device, VmaAllocator allocator);

private:
    vkb::Device _device;

    VkSampler _sampler;
};
