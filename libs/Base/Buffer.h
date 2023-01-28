#pragma once

#include "VkBootstrap.h"
#include "vk_mem_alloc.h"
#include "VkResource.h"

class Buffer {
public:
    Buffer() = default;

    Buffer(const vkb::Device &device, VmaAllocator allocator, VkCommandPool commandPool, VkBufferUsageFlags usage,
           uint32_t size, const void *data)
            : _allocator(allocator), _buffer{}, _sizeBytes{size} {
        VkBufferCreateInfo bufferCreateInfo{
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = {},
                .size = size,
                .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 0,
                .pQueueFamilyIndices = nullptr,
        };

        VmaAllocationCreateInfo allocationCreateInfo{
                .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
                .usage = VMA_MEMORY_USAGE_CPU_ONLY,
                .requiredFlags = {},
                .preferredFlags = {},
                .memoryTypeBits = {},
                .pool = nullptr,
                .pUserData = nullptr,
        };

        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferAlloc;
        VmaAllocationInfo stagingBufferAllocInfo;
        if (vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &stagingBuffer, &stagingBufferAlloc,
                            &stagingBufferAllocInfo) != VK_SUCCESS) {
            throw std::runtime_error("Error: vmaCreateBuffer");
        }

        memcpy(stagingBufferAllocInfo.pMappedData, data, size);

        // No need to flush stagingVertexBuffer memory because CPU_ONLY memory is always HOST_COHERENT.

        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocationCreateInfo.flags = 0;

        if (vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &_buffer, &_allocation, nullptr) !=
            VK_SUCCESS) {
            throw std::runtime_error("Error: vmaCreateBuffer");
        }

        VkCommandBufferAllocateInfo commandBufferInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = commandPool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1,
        };

        VkCommandBuffer transferCommandBuffer;
        if (vkAllocateCommandBuffers(device.device, &commandBufferInfo, &transferCommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkAllocateCommandBuffers");
        }

        VkCommandBufferBeginInfo transferCmdBeginInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = nullptr,
        };

        if (vkBeginCommandBuffer(transferCommandBuffer, &transferCmdBeginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkBeginCommandBuffer");
        }

        VkBufferCopy bufferCopy{
                .srcOffset = 0,
                .dstOffset = 0,
                .size = size,
        };
        vkCmdCopyBuffer(transferCommandBuffer, stagingBuffer, _buffer, 1, &bufferCopy);

        if (vkEndCommandBuffer(transferCommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkEndCommandBuffer");
        }

        VkSubmitInfo submitTransferInfo{
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext = nullptr,
                .waitSemaphoreCount = 0,
                .pWaitSemaphores = nullptr,
                .pWaitDstStageMask = nullptr,
                .commandBufferCount = 1,
                .pCommandBuffers = &transferCommandBuffer,
                .signalSemaphoreCount = 0,
                .pSignalSemaphores = nullptr,
        };

        if (vkQueueSubmit(device.get_queue(vkb::QueueType::graphics).value(), 1, &submitTransferInfo, VK_NULL_HANDLE) !=
            VK_SUCCESS) {
            throw std::runtime_error("Error: vkQueueSubmit");
        }

        if (vkQueueWaitIdle(device.get_queue(vkb::QueueType::graphics).value()) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkQueueWaitIdle");
        }

        vkFreeCommandBuffers(device.device, commandPool, 1, &transferCommandBuffer);
        vmaDestroyBuffer(allocator, stagingBuffer, stagingBufferAlloc);
    }

    Buffer(const Buffer &other) = delete;

    Buffer operator=(const Buffer &other) = delete;

    Buffer(Buffer &&other) = default;

    Buffer &operator=(Buffer &&other) = default;

    [[nodiscard]] VkBuffer getBuffer() const {
        return _buffer;
    }

    void destroy(const VkDevice device, const VmaAllocator &allocator) {
        vmaDestroyBuffer(_allocator, _buffer, _allocation);
    }

private:
    VmaAllocator _allocator;

    VkBuffer _buffer;
    VmaAllocation _allocation;

    uint32_t _sizeBytes;
};
