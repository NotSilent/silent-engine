#pragma once
#include <glm/mat4x4.hpp>

#include "vk-bootstrap/VkBootstrap.h"
#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan.h"
#include <glm/vec3.hpp>
#include <stdexcept>
#include <vector>


#include "Vertex.h"
#include "VkInitPipeline.h"

struct PushData {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 viewPosition;
};

// TODO: MeshFactory to hold data about device, allocator, pipeline data, renderpass, sizes and command pool, and also destroying resources

template<typename T>
class Buffer {
public:
    Buffer<T>() = default;

    Buffer<T>(const vkb::Device& device, const VmaAllocator allocator, const VkCommandPool commandPool, VkBufferUsageFlagBits usage, const uint32_t size, const T* data) : _buffer{}, _allocation{}
    {
        _allocator = allocator;

        VkBufferCreateInfo bufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .size = size,
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

        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferAlloc;
        VmaAllocationInfo stagingBufferAllocInfo;
        if (vmaCreateBuffer(_allocator, &bufferCreateInfo, &allocationCreateInfo, &stagingBuffer, &stagingBufferAlloc, &stagingBufferAllocInfo) != VK_SUCCESS) {
            throw std::runtime_error("Error: vmaCreateBuffer");
        }

        memcpy(stagingBufferAllocInfo.pMappedData, data, size);

        // No need to flush stagingVertexBuffer memory because CPU_ONLY memory is always HOST_COHERENT.

        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocationCreateInfo.flags = 0;

        if (vmaCreateBuffer(_allocator, &bufferCreateInfo, &allocationCreateInfo, &_buffer, &_allocation, nullptr) != VK_SUCCESS) {
            throw std::runtime_error("Error: vmaCreateBuffer");
        }

        VkCommandBufferAllocateInfo commandBufferInfo {
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
            .size = size,
        };
        vkCmdCopyBuffer(transferCommandBuffer, stagingBuffer, _buffer, 1, &bufferCopy);

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

        if (vkQueueSubmit(device.get_queue(vkb::QueueType::graphics).value(), 1, &submitTransferInfo, nullptr) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkQueueSubmit");
        }
        if (vkQueueWaitIdle(device.get_queue(vkb::QueueType::graphics).value()) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkQueueWaitIdle");
        }

        vmaDestroyBuffer(_allocator, stagingBuffer, stagingBufferAlloc);
    }

    void destroy()
    {
        vmaDestroyBuffer(_allocator, _buffer, _allocation);
    }

    VkBuffer getBuffer() const
    {
        return _buffer;
    }

private:
    VmaAllocator _allocator;

    VkBuffer _buffer;
    VmaAllocation _allocation;
};

class Mesh {
public:
    Mesh(const vkb::Device& device, const VmaAllocator allocator, const VkPipelineLayout pipelineLayout, const VkRenderPass renderPass,
        const uint32_t width, const uint32_t height, const VkCommandPool commandPool, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        _device = device;
        _pipelineLayout = pipelineLayout;

        _allocator = allocator;

        _vertexCount = vertices.size();
        _indexCount = indices.size();
        
        uint32_t vertexBufferSize = sizeof(Vertex) * _vertexCount;
        uint32_t indexBufferSize = sizeof(uint32_t) * _indexCount;
        
        _vertexBuffer = Buffer<Vertex>(device, allocator, commandPool, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferSize, vertices.data());
        _indexBuffer = Buffer<uint32_t>(device, allocator, commandPool, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferSize, indices.data());

        _pipeline = VkInit::Pipeline::createDefaultPipeline(device, pipelineLayout, renderPass, width, height);
    }

    ~Mesh()
    {
        _vertexBuffer.destroy();
        _indexBuffer.destroy();
        vkDestroyPipeline(_device.device, _pipeline, nullptr);
    }

    uint32_t getVertexCount() const
    {
        return _vertexCount;
    }

    VkBuffer getVertexBuffer() const
    {
        return _vertexBuffer.getBuffer();
    }

    uint32_t getIndexCount() const
    {
        return _indexCount;
    }

    VkBuffer getIndexBuffer() const
    {
        return _indexBuffer.getBuffer();
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
    Buffer<Vertex> _vertexBuffer;
    
    uint32_t _indexCount;
    Buffer<uint32_t> _indexBuffer;
};