#include "FrameResources.h"

#include "Image.h"
#include "VkInit.h"
#include "PushData.h"
#include "DrawData.h"
#include <limits>

FrameSynchronization::FrameSynchronization(VkDevice device)
        : queueFence(VkInit::createFence(device, VK_FENCE_CREATE_SIGNALED_BIT)), imageAcquireSemaphore(nullptr),
          presentSemaphore(nullptr) {
}

void FrameSynchronization::destroy(VkDevice device) {
    vkWaitForFences(device, 1, &queueFence, true, std::numeric_limits<uint64_t>::max());
    vkDestroyFence(device, queueFence, nullptr);
    vkDestroySemaphore(device, imageAcquireSemaphore, nullptr);
    vkDestroySemaphore(device, presentSemaphore, nullptr);
}

FrameResources::FrameResources(VkDevice device,
                               VmaAllocator allocator,
                               uint32_t queueFamilyIndex,
                               Image swapchainImage,
                               PipelineManager& pipelineManager,
                               const VkRect2D &renderArea)
        : device(device), swapchainImage(std::move(swapchainImage)),
          cmdPool(VkInit::createCommandPool(device, queueFamilyIndex)), synchronization(device),
          deferredRenderPass(device, allocator, renderArea),
          deferredLightningRenderPass(pipelineManager, renderArea, deferredRenderPass.getOutput()) {
    VkCommandBufferAllocateInfo allocateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };
    vkAllocateCommandBuffers(device, &allocateInfo, &cmd);
}

void FrameResources::destroy() {
    synchronization.destroy(device);

    vkResetCommandPool(device, cmdPool, {});
    vkDestroyCommandPool(device, cmdPool, nullptr);

    deferredRenderPass.destroy();
}

void FrameResources::renderFrame(VkSwapchainKHR swapchain, VkQueue graphicsQueue, uint32_t imageIndex,
                                 VkSemaphore imageAcquireSemaphore, VkFence presentFence, const DrawData &drawData) {
    VkSemaphore presentSemaphore = VkInit::createSemaphore(device);

    vkWaitForFences(device, 1, &synchronization.queueFence, true, std::numeric_limits<uint64_t>::max());
    vkResetFences(device, 1, &synchronization.queueFence);

    vkResetCommandPool(device, cmdPool, {});

    VkCommandBufferBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
    };

    vkBeginCommandBuffer(cmd, &beginInfo);

    deferredRenderPass.render(cmd, [&drawData](VkCommandBuffer commandBuffer) {
        for (const DrawCall &drawCall: drawData.getDrawCalls()) {
            // TODO: Move inside render pass
            PushData pushData(drawCall.model, drawData.view, drawData.projection);
            vkCmdPushConstants(commandBuffer, drawData.deferredPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                               sizeof(PushData),
                               &pushData);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawCall.pipeline);

            VkDeviceSize offset = 0;
            VkBuffer vertexBuffer = drawCall._mesh->getVertexBuffer();
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);

            vkCmdBindIndexBuffer(commandBuffer, drawCall._mesh->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(commandBuffer, drawCall._mesh->getIndexCount(), 1, 0, 0, 0);
        }
    });

    deferredLightningRenderPass.render(cmd, swapchainImage);

    vkEndCommandBuffer(cmd);

    VkSemaphoreSubmitInfo imageAcquireSemaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = imageAcquireSemaphore,
            .stageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };

    VkCommandBufferSubmitInfo cmdBufferSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = cmd,
    };

    VkSemaphoreSubmitInfo presentSemaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = presentSemaphore,
            .stageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    };

    VkSubmitInfo2 submitInfo2{
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
            .pWaitSemaphores = &presentSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &imageIndex,
            .pResults = nullptr,
    };

    vkQueuePresentKHR(graphicsQueue, &presentInfo);

    vkWaitForFences(device, 1, &presentFence, true, std::numeric_limits<uint64_t>::max());
    vkResetFences(device, 1, &presentFence);

    vkDestroySemaphore(device, synchronization.imageAcquireSemaphore, nullptr);
    vkDestroySemaphore(device, synchronization.presentSemaphore, nullptr);

    synchronization.imageAcquireSemaphore = imageAcquireSemaphore;
    synchronization.presentSemaphore = presentSemaphore;
}
