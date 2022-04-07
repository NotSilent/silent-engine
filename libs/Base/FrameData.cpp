#include "FrameData.h"


FrameData::FrameData(VkDevice device)
        : _device(device) {
}

FrameData::~FrameData() {
    reset();
}

void FrameData::addFence(VkFence fence) {
    _fences.push_back(fence);
}

void FrameData::addSemaphore(VkSemaphore semaphore) {
    _semaphores.push_back(semaphore);
}

void FrameData::wait() {

    if (!_fences.empty()) {
        vkWaitForFences(_device, _fences.size(), _fences.data(), VK_TRUE, std::numeric_limits<uint64_t>::max());
    }
}

void FrameData::reset() {
    for (VkFence fence: _fences) {
        vkDestroyFence(_device, fence, nullptr);
    }

    for (VkSemaphore semaphore: _semaphores) {
        vkDestroySemaphore(_device, semaphore, nullptr);
    }

    for (Command &command: _commands) {
        vkFreeCommandBuffers(_device, command._pool, 1, &command._buffer);
    }

    _fences.clear();
    _semaphores.clear();
    _commands.clear();
}

void FrameData::addCommand(VkCommandPool commandPool, VkCommandBuffer commandBuffer) {
    _commands.emplace_back(Command{commandPool, commandBuffer});
}
