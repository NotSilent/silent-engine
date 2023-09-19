#include "SynchronizationManager.h"

SynchronizationManager::SynchronizationManager(VkDevice device)
        : device(device)
        , stop(false)
        , worker(std::thread(&SynchronizationManager::work, this)) {
}

void SynchronizationManager::destroy() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();

    worker.join();
}

void SynchronizationManager::work() {
    while (true) {
        std::function<void(VkDevice)> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });

            if (stop && tasks.empty())
                return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        task(device);
    }
}

void SynchronizationManager::enqueue(const std::function<void(VkDevice)> &task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(task);
    }
    condition.notify_one();
}
