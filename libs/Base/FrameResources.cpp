#include "FrameResources.h"

#include "Image.h"
#include "VkInit.h"
#include "PushData.h"
#include "DrawData.h"
#include <limits>

FrameSynchronization::FrameSynchronization(VkDevice device)
    : queueFence(VkInit::createFence(device, VK_FENCE_CREATE_SIGNALED_BIT))
    , imageAcquireSemaphore(nullptr)
    , presentSemahore(nullptr) {
}

void FrameSynchronization::destroy(VkDevice device) {
    vkWaitForFences(device, 1, &queueFence, true, std::numeric_limits<uint64_t>::max());
    vkDestroyFence(device, queueFence, nullptr);
    vkDestroySemaphore(device, imageAcquireSemaphore, nullptr);
    vkDestroySemaphore(device, presentSemahore, nullptr);
}

FrameResources::FrameResources(VkDevice device,
                               VmaAllocator allocator,
                               uint32_t queueFamilyIndex,
                               VkImage swapchainImage,
                               VkImageView swapchainImageView,
                               const VkRect2D &renderArea)
        : device(device)
        , allocator(allocator)
        , queueFamilyIndex(queueFamilyIndex)
        , cmdPool(VkInit::createCommandPool(device, queueFamilyIndex))
        , synchronization(device)
        , deferredRenderPass(device, allocator, renderArea, swapchainImage, swapchainImageView) {
    VkCommandBufferAllocateInfo allocateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };
    vkAllocateCommandBuffers(device, &allocateInfo, &cmd);
}

FrameResources::FrameResources(FrameResources &&other) noexcept
    : synchronization(other.synchronization)
    , deferredRenderPass(std::move(other.deferredRenderPass)) {
    device = other.device;
    allocator = other.allocator;

    queueFamilyIndex = other.queueFamilyIndex;
    cmdPool = other.cmdPool;
    cmd = other.cmd;
}

FrameResources &FrameResources::operator=(FrameResources &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    device = other.device;
    allocator = other.allocator;

    queueFamilyIndex = other.queueFamilyIndex;
    cmdPool = other.cmdPool;
    cmd = other.cmd;

    synchronization = other.synchronization;

    deferredRenderPass = std::move(other.deferredRenderPass);

    return *this;
}

void FrameResources::destroy() {
    synchronization.destroy(device);

    vkResetCommandPool(device, cmdPool, {});
    vkDestroyCommandPool(device, cmdPool, nullptr);

    deferredRenderPass.destroy();
}

void FrameResources::renderFrame(VkSwapchainKHR swapchain, VkQueue graphicsQueue, uint32_t imageIndex, VkSemaphore imageAcquireSemaphore, const DrawData& drawData, VkRect2D renderArea) {
    vkWaitForFences(device, 1, &synchronization.queueFence, true, std::numeric_limits<uint64_t>::max());
    vkResetFences(device, 1, &synchronization.queueFence);

    vkResetCommandPool(device, cmdPool, {});

    vkDestroySemaphore(device, synchronization.imageAcquireSemaphore, nullptr);
    vkDestroySemaphore(device, synchronization.presentSemahore, nullptr);

    synchronization.imageAcquireSemaphore = imageAcquireSemaphore;
    synchronization.presentSemahore = VkInit::createSemaphore(device);

    VkCommandBufferBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
    };

    vkBeginCommandBuffer(cmd, &beginInfo);

    deferredRenderPass.render(cmd, [&drawData](VkCommandBuffer commandBuffer) {
        for (const DrawCall &drawCall: drawData.getDrawCalls()) {
            PushData pushData(drawCall.model, drawData.view, drawData.projection);

            vkCmdPushConstants(commandBuffer, drawCall.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushData),
                               &pushData);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawCall.pipeline);

            VkDeviceSize offset = 0;
            VkBuffer vertexBuffer = drawCall._mesh->getVertexBuffer();
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);

            vkCmdBindIndexBuffer(commandBuffer, drawCall._mesh->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(commandBuffer, drawCall._mesh->getIndexCount(), 1, 0, 0, 0);
        }
    });

    vkEndCommandBuffer(cmd);

     VkSemaphoreSubmitInfo imageAcquireSemaphoreSubmitInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = imageAcquireSemaphore,
        .stageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };

     VkCommandBufferSubmitInfo cmdBufferSubmitInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = cmd,
    };

    VkSemaphoreSubmitInfo presentSemaphoreSubmitInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = synchronization.presentSemahore,
            .stageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    };

    VkSubmitInfo2 submitInfo2 {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = 1,
        .pWaitSemaphoreInfos = &imageAcquireSemaphoreSubmitInfo,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &cmdBufferSubmitInfo,
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos = &presentSemaphoreSubmitInfo,
    };

    vkQueueSubmit2(graphicsQueue, 1, &submitInfo2, synchronization.queueFence);

    VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &synchronization.presentSemahore,
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &imageIndex,
            .pResults = nullptr,
    };

    vkQueuePresentKHR(graphicsQueue, &presentInfo);
}
