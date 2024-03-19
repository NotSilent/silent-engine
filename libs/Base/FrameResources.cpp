#include "FrameResources.h"

#include "Image.h"
#include "VkInit.h"
#include "PushData.h"
#include "DrawData.h"
#include <limits>

FrameSynchronization::FrameSynchronization(vk::Device device)
        : queueFence(VkInit::createFence(device, vk::FenceCreateFlagBits::eSignaled)), imageAcquireSemaphore(nullptr),
          presentSemaphore(nullptr) {
}

void FrameSynchronization::destroy(vk::Device device) {
    device.waitForFences(1, &queueFence, true, std::numeric_limits<uint64_t>::max());
    device.destroyFence(queueFence);
    device.destroySemaphore(imageAcquireSemaphore);
    device.destroySemaphore(presentSemaphore);
}

FrameResources::FrameResources(vk::Device device,
                               VmaAllocator allocator,
                               uint32_t queueFamilyIndex,
                               Image swapchainImage,
                               PipelineManager &pipelineManager,
                               const vk::Rect2D &renderArea)
        : device(device), swapchainImage(std::move(swapchainImage)),
          cmdPool(VkInit::createCommandPool(device, queueFamilyIndex)), synchronization(device),
          shadowMapRenderpass(device, allocator, pipelineManager),
          deferredRenderPass(device, allocator, renderArea),
          deferredLightningRenderPass(pipelineManager, renderArea, deferredRenderPass.getOutput(),
                                      shadowMapRenderpass.getOutput()) {

    vk::CommandBufferAllocateInfo allocateInfo{cmdPool, vk::CommandBufferLevel::ePrimary, 1};
    device.allocateCommandBuffers(&allocateInfo, &cmd);
}

void FrameResources::destroy() {
    synchronization.destroy(device);

    device.resetCommandPool(cmdPool, {});
    device.destroyCommandPool(cmdPool);

    shadowMapRenderpass.destroy();
    deferredRenderPass.destroy();
}

void FrameResources::renderFrame(vk::SwapchainKHR swapchain, vk::Queue graphicsQueue, uint32_t imageIndex,
                                 vk::Semaphore imageAcquireSemaphore, vk::Fence presentFence,
                                 const DrawData &drawData) {
    // TODO: draw code inside renderpasses

    vk::Semaphore presentSemaphore = VkInit::createSemaphore(device);

    device.waitForFences(1, &synchronization.queueFence, true, std::numeric_limits<uint64_t>::max());
    device.resetFences(1, &synchronization.queueFence);

    device.resetCommandPool(cmdPool, {});

    vk::CommandBufferBeginInfo beginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr};

    cmd.begin(&beginInfo);

    deferredRenderPass.render(cmd, drawData);

    shadowMapRenderpass.render(cmd, drawData);

    const glm::vec3 viewDirection = glm::inverse(-drawData.view)[2];

    deferredLightningRenderPass.render(cmd, swapchainImage,
                                       drawData.directionalLight.getProjection() * drawData.directionalLight.getView(),
                                       viewDirection);

    cmd.end();

    vk::SemaphoreSubmitInfo imageAcquireSemaphoreSubmitInfo{
            imageAcquireSemaphore,
            0,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            0
    };

    vk::CommandBufferSubmitInfo cmdBufferSubmitInfo{cmd};

    vk::SemaphoreSubmitInfo presentSemaphoreSubmitInfo{
            presentSemaphore,
            0,
            vk::PipelineStageFlagBits2::eBottomOfPipe,
            0
    };

    vk::SubmitInfo2 submitInfo2{
            {},
            1, &imageAcquireSemaphoreSubmitInfo,
            1, &cmdBufferSubmitInfo,
            1, &presentSemaphoreSubmitInfo,
    };

    graphicsQueue.submit2(1, &submitInfo2, synchronization.queueFence);

    vk::PresentInfoKHR presentInfo{
            1, &presentSemaphore,
            1, &swapchain,
            &imageIndex,
            nullptr
    };

    graphicsQueue.presentKHR(&presentInfo);

    device.waitForFences(1, &presentFence, true, std::numeric_limits<uint64_t>::max());
    device.resetFences(1, &presentFence);

    device.destroySemaphore(synchronization.imageAcquireSemaphore);
    device.destroySemaphore(synchronization.presentSemaphore);

    synchronization.imageAcquireSemaphore = imageAcquireSemaphore;
    synchronization.presentSemaphore = presentSemaphore;
}
