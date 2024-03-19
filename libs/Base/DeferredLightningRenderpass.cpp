#include "DeferredLightningRenderpass.h"
#include "CommandBuffer.h"

DeferredLightningRenderpass::DeferredLightningRenderpass(PipelineManager &pipelineManager,
                                                         vk::Rect2D renderArea,
                                                         const DeferredRenderPassOutput &deferredRenderPassOutput,
                                                         const ShadowMapRenderPassOutput &shadowMapRenderPassOutput)
        : renderArea(renderArea)
        , deferredRenderPassOutput(deferredRenderPassOutput)
        , shadowMapRenderPassOutput(shadowMapRenderPassOutput)
        , material(pipelineManager.createDeferredLightningMaterial(deferredRenderPassOutput.color.imageView,
                                                                   deferredRenderPassOutput.normal.imageView,
                                                                   deferredRenderPassOutput.position.imageView,
                                                                   shadowMapRenderPassOutput.depth.imageView)) {
}

void DeferredLightningRenderpass::render(vk::CommandBuffer cmd, const Image &swapchainImage, const glm::mat4& lightSpace, glm::vec3 viewDirection) {
    beginRenderPass(cmd, swapchainImage);

    struct LightningPushData {
        glm::mat4 lightSpace = {};
        glm::vec3 view = {};
    } pushData;

    pushData.lightSpace = lightSpace;
    pushData.view = viewDirection;

    cmd.pushConstants(material.layout, vk::ShaderStageFlagBits::eFragment, 0, sizeof(LightningPushData), &pushData);
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, material.pipeline);
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, material.layout, 0, 1, &material.set, 0, nullptr);

    cmd.draw(6, 1, 0, 0);

    endRenderPass(cmd, swapchainImage);
}

void DeferredLightningRenderpass::beginRenderPass(vk::CommandBuffer cmd, const Image &swapchainImage) {
    vk::RenderingAttachmentInfo swapchainAttachment(
            swapchainImage.getImageView(),
            vk::ImageLayout::eAttachmentOptimal,
            vk::ResolveModeFlagBits::eNone,
            nullptr,
            vk::ImageLayout::eUndefined,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            swapchainClearValue
    );

    std::array attachments{swapchainAttachment};

    CommandBuffer::pipelineBarrier(cmd, vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                   vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                   vk::AccessFlagBits::eNone,
                                   vk::AccessFlagBits::eColorAttachmentWrite,
                                   vk::ImageLayout::eUndefined,
                                   vk::ImageLayout::eColorAttachmentOptimal,
                                   swapchainImage.getImage(),
                                   vk::ImageAspectFlagBits::eColor);

    CommandBuffer::pipelineBarrier(cmd, vk::PipelineStageFlagBits::eTopOfPipe,
                                   vk::PipelineStageFlagBits::eFragmentShader,
                                   vk::AccessFlagBits::eNone,
                                   vk::AccessFlagBits::eShaderRead,
                                   deferredRenderPassOutput.color.imageLayout,
                                   vk::ImageLayout::eShaderReadOnlyOptimal,
                                   deferredRenderPassOutput.color.image,
                                   vk::ImageAspectFlagBits::eColor);

    CommandBuffer::pipelineBarrier(cmd, vk::PipelineStageFlagBits::eTopOfPipe,
                                   vk::PipelineStageFlagBits::eFragmentShader,
                                   vk::AccessFlagBits::eNone,
                                   vk::AccessFlagBits::eShaderRead,
                                   deferredRenderPassOutput.normal.imageLayout,
                                   vk::ImageLayout::eShaderReadOnlyOptimal,
                                   deferredRenderPassOutput.normal.image,
                                   vk::ImageAspectFlagBits::eColor);

    CommandBuffer::pipelineBarrier(cmd, vk::PipelineStageFlagBits::eTopOfPipe,
                                   vk::PipelineStageFlagBits::eFragmentShader,
                                   vk::AccessFlagBits::eNone,
                                   vk::AccessFlagBits::eShaderRead,
                                   deferredRenderPassOutput.position.imageLayout,
                                   vk::ImageLayout::eShaderReadOnlyOptimal,
                                   deferredRenderPassOutput.position.image,
                                   vk::ImageAspectFlagBits::eColor);

    CommandBuffer::pipelineBarrier(cmd, vk::PipelineStageFlagBits::eTopOfPipe,
                                   vk::PipelineStageFlagBits::eFragmentShader,
                                   vk::AccessFlagBits::eNone,
                                   vk::AccessFlagBits::eShaderRead,
                                   shadowMapRenderPassOutput.depth.imageLayout,
                                   vk::ImageLayout::eShaderReadOnlyOptimal,
                                   shadowMapRenderPassOutput.depth.image,
                                   vk::ImageAspectFlagBits::eDepth);

    vk::RenderingInfo renderingInfo(
            {},
            renderArea,
            1,
            0,
            attachments.size(),
            attachments.data(),
            nullptr,
            nullptr
    );

    cmd.beginRendering(&renderingInfo);
}

void DeferredLightningRenderpass::endRenderPass(vk::CommandBuffer cmd, const Image &swapchainImage) {
    cmd.endRendering();

    CommandBuffer::pipelineBarrier(cmd, vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                   vk::PipelineStageFlagBits::eBottomOfPipe,
                                   vk::AccessFlagBits::eColorAttachmentWrite,
                                   vk::AccessFlagBits::eNone,
                                   vk::ImageLayout::eColorAttachmentOptimal,
                                   vk::ImageLayout::ePresentSrcKHR,
                                   swapchainImage.getImage(),
                                   vk::ImageAspectFlagBits::eColor);
}
