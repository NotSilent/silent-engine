#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vulkan/vulkan_core.h>

class SynchronizationManager {
public:
    explicit SynchronizationManager(VkDevice device);

    SynchronizationManager(SynchronizationManager& other) = delete;
    SynchronizationManager& operator=(SynchronizationManager& other) = delete;

    SynchronizationManager(SynchronizationManager&& other) = delete;
    SynchronizationManager& operator=(SynchronizationManager&& other) = delete;

    void destroy();

    void work();

    void enqueue(const std::function<void(VkDevice)>& task);

private:
    VkDevice device;

    std::thread worker;

    std::queue<std::function<void(VkDevice)>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;

    bool stop;
};