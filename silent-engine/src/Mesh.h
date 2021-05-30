#pragma once
#include "vk-bootstrap/VkBootstrap.h"
#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan.h"
#include <glm/vec3.hpp>
#include <stdexcept>
#include <vector>

#include "VkInitPipeline.h"

struct Vertex {
    glm::vec3 position;
};

struct PushData {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

class Mesh {
public:
    Mesh(const vkb::Device& device, const VmaAllocator allocator, const VkPipelineLayout pipelineLayout, const VkRenderPass renderPass,
        const uint32_t width, const uint32_t height, const VkCommandPool commandPool, const std::vector<Vertex>& vertices)
    {
        _device = device;
        _pipelineLayout = pipelineLayout;

        _allocator = allocator;

        _vertexCount = vertices.size();

        size_t vertexBufferSize = sizeof(Vertex) * _vertexCount;

        VkBufferCreateInfo bufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .size = vertexBufferSize,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
        };

        VmaAllocationCreateInfo allocationCreateInfo {
            .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_CPU_ONLY,
            .requiredFlags = {},
            .preferredFlags = {},
            .memoryTypeBits = {},
            .pool = nullptr,
            .pUserData = nullptr,
        };

        VkBuffer stagingVertexBuffer;
        VmaAllocation stagingVertexBufferAlloc;
        VmaAllocationInfo stagingVertexBufferAllocInfo;
        if (vmaCreateBuffer(_allocator, &bufferCreateInfo, &allocationCreateInfo, &stagingVertexBuffer, &stagingVertexBufferAlloc, &stagingVertexBufferAllocInfo) != VK_SUCCESS) {
            throw std::runtime_error("Error: vmaCreateBuffer");
        }

        memcpy(stagingVertexBufferAllocInfo.pMappedData, vertices.data(), vertexBufferSize);

        // No need to flush stagingVertexBuffer memory because CPU_ONLY memory is always HOST_COHERENT.

        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocationCreateInfo.flags = 0;

        VkBuffer vertexBuffer;
        VmaAllocation vertexBufferAlloc;
        VmaAllocationInfo vertexBufferAllocInfo;
        if (vmaCreateBuffer(_allocator, &bufferCreateInfo, &allocationCreateInfo, &vertexBuffer, &vertexBufferAlloc, nullptr) != VK_SUCCESS) {
            throw std::runtime_error("Error: vmaCreateBuffer");
        }

        _vertexBuffer = vertexBuffer;
        _vertexBufferAllocation = vertexBufferAlloc;

        VkCommandBufferAllocateInfo commandBufferInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        VkCommandBuffer transferCommandBuffer;
        if (vkAllocateCommandBuffers(_device.device, &commandBufferInfo, &transferCommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkAllocateCommandBuffers");
        }

        VkCommandBufferBeginInfo transferCmdBeginInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
        };

        if (vkBeginCommandBuffer(transferCommandBuffer, &transferCmdBeginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkBeginCommandBuffer");
        }

        VkBufferCopy bufferCopy {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = vertexBufferSize,
        };
        vkCmdCopyBuffer(transferCommandBuffer, stagingVertexBuffer, vertexBuffer, 1, &bufferCopy);

        if (vkEndCommandBuffer(transferCommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkEndCommandBuffer");
        }

        VkSubmitInfo submitTransferInfo {
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

        if (vkQueueSubmit(_device.get_queue(vkb::QueueType::graphics).value(), 1, &submitTransferInfo, nullptr) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkQueueSubmit");
        }
        if (vkQueueWaitIdle(_device.get_queue(vkb::QueueType::graphics).value()) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkQueueWaitIdle");
        }

        vmaDestroyBuffer(allocator, stagingVertexBuffer, stagingVertexBufferAlloc);

        _pipeline = VkInit::Pipeline::createDefaultPipeline(device, pipelineLayout, renderPass, width, height);
    }

    ~Mesh()
    {
        vkDestroyPipeline(_device.device, _pipeline, nullptr);
        vmaDestroyBuffer(_allocator, _vertexBuffer, _vertexBufferAllocation);
    }

    uint32_t getVertexCount() const
    {
        return _vertexCount;
    }

    VkBuffer getVertexBuffer() const
    {
        return _vertexBuffer;
    }

    VkPipelineLayout getPipelineLayout() const
    {
        return _pipelineLayout;
    }

    VkPipeline getPipeline() const
    {
        return _pipeline;
    }

private:
    vkb::Device _device;

    VkPipelineLayout _pipelineLayout;
    VkPipeline _pipeline;

    VmaAllocator _allocator;

    uint32_t _vertexCount;
    VkBuffer _vertexBuffer;
    VmaAllocation _vertexBufferAllocation;
};