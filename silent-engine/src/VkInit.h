#pragma once
#include <vulkan/vulkan.h>

#include "vk-bootstrap/VkBootstrap.h"
#include "vma/vk_mem_alloc.h"

namespace VkInit {
VmaAllocator createAllocator(const vkb::Instance& instance, const vkb::PhysicalDevice& physicalDevice, const vkb::Device& device, uint32_t vulkanApiVersion, const uint32_t frameInUseCount);

VkDescriptorPool createDescriptorPool(const vkb::Device& device);

VkDescriptorSetLayout createDefaultDescriptorSetLayout(const vkb::Device& device);

VkDescriptorSet createDescriptorSet(const vkb::Device& device, VkDescriptorPool pool, VkDescriptorSetLayout layout, VkSampler sampler, VkImageView imageView);

VkSemaphore createSemaphore(const vkb::Device& device);

VkFence createFence(const vkb::Device& device);

VkFramebuffer createFramebuffer(const vkb::Device& device, const VkRenderPass renderPass, const VkImageView color, VkImageView depthStencil, const uint32_t width, const uint32_t height);

VkCommandPool createCommandPool(const vkb::Device& device);

VkRenderPass createRenderPass(const vkb::Device& device, const vkb::Swapchain& swapchain);
}
