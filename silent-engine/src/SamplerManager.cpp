#include "SamplerManager.h"

SamplerManager::SamplerManager(const vkb::Device& device, VmaAllocator allocator, VkCommandPool commandPool)
    : _device(device)
    , _allocator(allocator)
    , _commandPool(commandPool)
{
}

void SamplerManager::addSampler(const std::string& name)
{
    if (_samplers.contains(name)) {
        return;
    }

    _samplers[name] = std::make_shared<Sampler>(_device);
}

std::shared_ptr<Sampler> SamplerManager::getSampler(const std::string& name)
{
    // TODO: error checking
    return _samplers[name];
}

void SamplerManager::destroy()
{
    for (auto& sampler : _samplers) {
        sampler.second->destroy(_device.device, _allocator);
    }
}
