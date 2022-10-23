#pragma once

#include "vulkan/vulkan_core.h"
#include <vk_mem_alloc.h>

template<typename T>
class VkResource {
public:
	void destroy(VkDevice device, VmaAllocator allocator)
	{
	    static_cast<T>(*this).destroy(device, allocator);
	}
};