#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class FrameData {
    struct Command {
        VkCommandPool _pool;
        VkCommandBuffer _buffer;
    };

public:
    FrameData() = default;

    explicit FrameData(VkDevice device);

    ~FrameData();

    void addFence(VkFence fence);

    void addSemaphore(VkSemaphore semaphore);

    void addCommand(VkCommandPool commandPool, VkCommandBuffer commandBuffer);

    void wait();

    void reset();

private:
    VkDevice _device;
    std::vector<VkFence> _fences;
    std::vector<VkSemaphore> _semaphores;
    std::vector<Command> _commands;
};