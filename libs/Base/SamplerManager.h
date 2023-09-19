#pragma once

#include "vk_mem_alloc.h"
#include <memory>
#include <string>
#include <unordered_map>
#include "Sampler.h"

class SamplerManager {
public:
    SamplerManager() = default;

    SamplerManager(const vkb::Device &device, VmaAllocator allocator, VkCommandPool commandPool);

    void addSampler(const std::string &name);

    std::shared_ptr<Sampler> getSampler(const std::string &name);

    void destroy();

private:
    vkb::Device _device;
    VmaAllocator _allocator;
    VkCommandPool _commandPool;

    std::unordered_map<std::string, std::shared_ptr<Sampler>> _samplers{};
};
