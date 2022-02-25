#pragma once
#include <vulkan\vulkan.h>
#include <VkBootstrap.h>
#include "VkResource.h"

class Sampler : VkResource<Sampler> {
public:
	Sampler(const vkb::Device& device);
	~Sampler();

	VkSampler getSampler() const;

	void destroy(VkDevice device, VmaAllocator allocator);

private:
	vkb::Device _device;

	VkSampler _sampler;
};
