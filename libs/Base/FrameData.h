#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include "Image.h"
#include <memory>

struct GBufferResources {
    std::shared_ptr<Image> color;
    std::shared_ptr<Image> position;
    std::shared_ptr<Image> normal;
    std::shared_ptr<Image> depth;
};

class PipelineManager;

class DescriptorSetManager;

class DescriptorSetLayout;

class DescriptorSet;

class Pipeline;

class FrameData : public VkResource<FrameData> {
    struct Command {
        VkCommandPool _pool;
        VkCommandBuffer _buffer;
    };

public:
    // TODO: swapchainImageView is used to define framebuffer attachments
    FrameData(VkDevice device, VmaAllocator allocator, uint32_t width, uint32_t height,
              VkRenderPass compatibleRenderPass, VkImageView swapchainImageView,
              const std::shared_ptr<PipelineManager> &pipelineManager,
              const std::shared_ptr<DescriptorSetManager> &descriptorSetManager);

    void destroy(VkDevice device, VmaAllocator allocator);

    void addFence(VkFence fence);

    void addSemaphore(VkSemaphore semaphore);

    void addCommand(VkCommandPool commandPool, VkCommandBuffer commandBuffer);

    void wait();

    void reset();

    void
    createGBuffer(VkDevice device, VmaAllocator allocator, uint32_t width, uint32_t height,
                  VkRenderPass renderPass, VkImageView swapchainImageView,
                  const std::shared_ptr<PipelineManager> &pipelineManager,
                  const std::shared_ptr<DescriptorSetManager> &descriptorSetManager);

    [[nodiscard]] VkFramebuffer getGBuffer() const;

    std::shared_ptr<DescriptorSetLayout> _compositeDescriptorSetLayout;
    std::shared_ptr<DescriptorSet> _compositeDescriptorSet;
    std::shared_ptr<Pipeline> _compositePipeline;

private:
    VkDevice _device = VK_NULL_HANDLE;

    GBufferResources _gBufferResources;
    VkFramebuffer _gBuffer = VK_NULL_HANDLE;

    std::vector<VkFence> _fences;
    std::vector<VkSemaphore> _semaphores;
    std::vector<Command> _commands;
};