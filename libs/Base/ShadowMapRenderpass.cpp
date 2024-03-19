#include "ShadowMapRenderpass.h"
#include "CommandBuffer.h"
#include "DeferredRenderpass.h"
#include "DrawData.h"
#include "PushData.h"
#include "PipelineManager.h"

ShadowMapRenderpass::ShadowMapRenderpass(vk::Device device, VmaAllocator allocator,
                                         const PipelineManager &pipelineManager)
        : device(device), allocator(allocator), material(pipelineManager.getShadowMapMaterial()) {
    depthImage = createDepthImage();
}

void ShadowMapRenderpass::destroy() {
    depthImage.destroy(device, allocator);
}

void ShadowMapRenderpass::beginRenderPass(vk::CommandBuffer cmd) {
    vk::RenderingAttachmentInfo depthAttachment{
            depthImage.getImageView(),
            vk::ImageLayout::eDepthAttachmentOptimal,
            vk::ResolveModeFlagBits::eNone,
            nullptr,
            vk::ImageLayout::eUndefined,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            DEPTH_CLEAR_VALUE
    };

    CommandBuffer::pipelineBarrier(
            cmd,
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eEarlyFragmentTests,
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eDepthStencilAttachmentWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthAttachmentOptimal,
            depthImage.getImage(),
            vk::ImageAspectFlagBits::eDepth
    );

    vk::RenderingInfo renderingInfo{
            {},
            SHADOW_MAP_DIMENSIONS,
            1,
            0,
            0,
            nullptr,
            &depthAttachment,
            nullptr
    };

    cmd.beginRendering(&renderingInfo);
}

void ShadowMapRenderpass::endRenderPass(vk::CommandBuffer cmd) {
    cmd.endRendering();
}

void ShadowMapRenderpass::render(vk::CommandBuffer cmd, const DrawData &drawData) {
    beginRenderPass(cmd);

    // Shadow PushData
    // Uniform

    const glm::mat4 view = drawData.directionalLight.getView();
    const glm::mat4 projection = drawData.directionalLight.getProjection();
    for (const DrawCall &drawCall: drawData.getDrawCalls()) {
        PushData pushData(drawCall.model, view, projection);
        cmd.pushConstants(material.layout, vk::ShaderStageFlagBits::eVertex, 0,
                          sizeof(PushData),
                          &pushData);

        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, material.pipeline);

        std::array buffers{
                drawCall._mesh->getPositionsBuffer(),
        };

        std::array<vk::DeviceSize, 2> offsets{
                0,
        };

        cmd.bindVertexBuffers(0, buffers.size(), buffers.data(), offsets.data());

        cmd.bindIndexBuffer(drawCall._mesh->getIndexBuffer(), 0, vk::IndexType::eUint32);

        cmd.drawIndexed(drawCall._mesh->getIndexCount(), 1, 0, 0, 0);
    }

    endRenderPass(cmd);
}

Image ShadowMapRenderpass::createDepthImage() const {
    ImageCreateInfo createInfo{
            {SHADOW_MAP_DIMENSIONS.extent.width, SHADOW_MAP_DIMENSIONS.extent.height, 1},
            vk::ImageType::e2D,
            DeferredRenderpassDefinitions::Formats::DEPTH,
            vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
            vk::ImageViewType::e2D,
            vk::ImageAspectFlagBits::eDepth
    };

    return {device, allocator, createInfo};
}

ShadowMapRenderPassOutput ShadowMapRenderpass::getOutput() const {
    RenderPassAttachmentOutput depth{depthImage.getImage(), depthImage.getImageView(),
                                     vk::ImageLayout::eDepthAttachmentOptimal};

    return ShadowMapRenderPassOutput{depth};
}
