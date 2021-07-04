#pragma once

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

template<class T>
class VkResource {
public:
	void destroy(VkDevice device, VmaAllocator allocator)
	{
	    static_cast<T>(*this).destroy(device, allocator);
	}
};