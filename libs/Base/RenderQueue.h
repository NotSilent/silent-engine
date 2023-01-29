#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

struct RenderResources {
    VkDevice device;

    uint32_t imageIndex;
    VkQueue graphicsQueue;

    VkSwapchainKHR swapchain;

    VkSemaphore imageAcquiredSemaphore;
    VkSemaphore submitSemaphore;
    VkFence queueSubmitFence;

    VkCommandPool cmdPool;
    VkCommandBuffer cmd;
};

class RenderQueue {
public:

    RenderQueue() {
        _thread = std::move(std::thread(&RenderQueue::work, this));
    }

    void addWork(const RenderResources &resources) {
        {
            std::unique_lock lock(workMutex);
            cv.wait(lock, [&]() { return _readyForMoreWork; });
        }

        {
            std::unique_lock lock(resourcesMutex);
            resourcesQueue.emplace_back(resources);
            _readyForMoreWork = resourcesQueue.size() < MAX_RESOURCES;
        }
    }

    void signalToStop() {
        _signaledToStop = true;
        _thread.join();
    }

private:
    std::thread _thread;

    static const size_t MAX_RESOURCES = 1;

    std::condition_variable cv;

    std::mutex workMutex;
    bool _readyForMoreWork = true;

    std::mutex resourcesMutex;
    std::vector<RenderResources> resourcesQueue;

    bool _signaledToStop = false;

    void work() {
        while (!_signaledToStop) {
            if (!resourcesQueue.empty()) {
                const RenderResources resource = resourcesQueue[0];
                {
                    {
                        std::unique_lock lock(resourcesMutex);
                        resourcesQueue.erase(resourcesQueue.begin());
                    }

                    {
                        std::unique_lock lock(workMutex);
                        _readyForMoreWork = resourcesQueue.size() < MAX_RESOURCES;
                    }
                    cv.notify_all();
                }

                render(resource);
                releaseResources(resource);
            } else {
                {
                    std::unique_lock lock(workMutex);
                    _readyForMoreWork = resourcesQueue.size() < MAX_RESOURCES;
                }
                cv.notify_all();
            }
        }
    }

    void render(const RenderResources &resources) {
        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo{
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext = nullptr,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &resources.imageAcquiredSemaphore,
                .pWaitDstStageMask = &waitDstStageMask,
                .commandBufferCount = 1,
                .pCommandBuffers = &resources.cmd,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = &resources.submitSemaphore,
        };

        vkQueueSubmit(resources.graphicsQueue, 1, &submitInfo, resources.queueSubmitFence);

        VkPresentInfoKHR presentInfo{
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .pNext = nullptr,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &resources.submitSemaphore,
                .swapchainCount = 1,
                .pSwapchains = &resources.swapchain,
                .pImageIndices = &resources.imageIndex,
                .pResults = nullptr,
        };

        vkQueuePresentKHR(resources.graphicsQueue, &presentInfo);

//        vkWaitForFences(resources.device, 1, &resources.queueSubmitFence, VK_TRUE,
//                        (std::numeric_limits<uint64_t>::max)());
    }

    void releaseResources(const RenderResources &resources) {
        //vkDestroySemaphore(resources.device, resources.imageAcquiredSemaphore, nullptr);
        //vkDestroySemaphore(resources.device, resources.submitSemaphore, nullptr);

        //vkDestroyFence(resources.device, resources.queueSubmitFence, nullptr);

        //vkFreeCommandBuffers(resources.device, resources.cmdPool, 1, &resources.cmd);
    }
};