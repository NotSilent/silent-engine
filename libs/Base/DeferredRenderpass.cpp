#include "DeferredRenderpass.h"

#include "CommandBuffer.h"
#include "PushData.h"
#include "DrawData.h"
#include "RenderPassAttachmentOutput.h"

DeferredRenderpass::DeferredRenderpass(vk::Device device,
                                       VmaAllocator allocator,
                                       vk::Rect2D renderArea)
        : device(device)
        , allocator(allocator)
        , renderArea(renderArea) {
    colorImage = createColorImage(DeferredRenderpassDefinitions::Formats::COLOR);
    normalImage = createColorImage(DeferredRenderpassDefinitions::Formats::NORMAL);
    positionImage = createColorImage(DeferredRenderpassDefinitions::Formats::POSITION);
    depthImage = createDepthImage();
}

void DeferredRenderpass::destroy() {
    colorImage.destroy(device, allocator);
    normalImage.destroy(device, allocator);
    positionImage.destroy(device, allocator);
    depthImage.destroy(device, allocator);
}

void DeferredRenderpass::render(vk::CommandBuffer cmd, const DrawData& drawData) {
    beginRenderPass(cmd);

    // TODO: Why tf is deferredPipelineLayout a part of DrawData

    for (const DrawCall &drawCall: drawData.getDrawCalls()) {
        PushData pushData(drawCall.model, drawData.view, drawData.projection);
        cmd.pushConstants(drawData.deferredPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0,
                           sizeof(PushData),
                           &pushData);

        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, drawCall.pipeline);

        std::array buffers {
                drawCall._mesh->getPositionsBuffer(),
                drawCall._mesh->getAttributesBuffer(),
        };

        std::array<vk::DeviceSize, 2> offsets {
                0,
                0,
        };

        cmd.bindVertexBuffers(0, buffers, offsets);

        cmd.bindIndexBuffer(drawCall._mesh->getIndexBuffer(), 0, vk::IndexType::eUint32);

        cmd.drawIndexed(drawCall._mesh->getIndexCount(), 1, 0, 0, 0);
    }

    endRenderPass(cmd);
}

void DeferredRenderpass::beginRenderPass(vk::CommandBuffer cmd) {
    vk::RenderingAttachmentInfo colorAttachment(
        colorImage.getImageView(),
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ResolveModeFlagBits::eNone,
        nullptr,
        vk::ImageLayout::eUndefined,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        CLEAR_VALUE
        );

    vk::RenderingAttachmentInfo normalAttachment(
        normalImage.getImageView(),
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ResolveModeFlagBits::eNone,
        nullptr,
        vk::ImageLayout::eUndefined,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        CLEAR_VALUE
    );

    vk::RenderingAttachmentInfo positionAttachment(
        positionImage.getImageView(),
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ResolveModeFlagBits::eNone,
        nullptr,
        vk::ImageLayout::eUndefined,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        CLEAR_VALUE
    );

    vk::RenderingAttachmentInfo depthAttachment(
        depthImage.getImageView(),
        vk::ImageLayout::eDepthAttachmentOptimal,
        vk::ResolveModeFlagBits::eNone,
        nullptr,
        vk::ImageLayout::eUndefined,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        DEPTH_CLEAR_VALUE
    );

    std::array attachments {colorAttachment, normalAttachment, positionAttachment};

    // TODO: Single vk::CmdPipelineBarrier

    CommandBuffer::pipelineBarrier(
            cmd,
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eColorAttachmentWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            colorImage.getImage(),
            vk::ImageAspectFlagBits::eColor
    );

    CommandBuffer::pipelineBarrier(
            cmd,
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eColorAttachmentWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            normalImage.getImage(),
            vk::ImageAspectFlagBits::eColor
    );

    CommandBuffer::pipelineBarrier(
            cmd,
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eColorAttachmentWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            positionImage.getImage(),
            vk::ImageAspectFlagBits::eColor
    );

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

    vk::RenderingInfo renderingInfo(
            {},
            renderArea,
            1,
            0,
            attachments.size(),
            attachments.data(),
            &depthAttachment,
            nullptr
    );

    cmd.beginRendering(&renderingInfo);
}

void DeferredRenderpass::endRenderPass(vk::CommandBuffer cmd) {
    cmd.endRendering();
}

Image DeferredRenderpass::createColorImage(vk::Format format) const {
    ImageCreateInfo createInfo {
        {renderArea.extent.width, renderArea.extent.height, 1},
        vk::ImageType::e2D,
        format,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eColor,
    };

    return Image(device, allocator, createInfo);
}

Image DeferredRenderpass::createDepthImage() const {
    ImageCreateInfo createInfo {
            {renderArea.extent.width, renderArea.extent.height, 1},
            vk::ImageType::e2D,
            DeferredRenderpassDefinitions::Formats::DEPTH,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::ImageViewType::e2D,
            vk::ImageAspectFlagBits::eDepth,
    };

    return Image(device, allocator, createInfo);
}

DeferredRenderPassOutput DeferredRenderpass::getOutput() const {
    RenderPassAttachmentOutput color = {
            .image = colorImage.getImage(),
            .imageView = colorImage.getImageView(),
            .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    RenderPassAttachmentOutput normal = {
            .image = normalImage.getImage(),
            .imageView = normalImage.getImageView(),
            .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    RenderPassAttachmentOutput position = {
            .image = positionImage.getImage(),
            .imageView = positionImage.getImageView(),
            .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    RenderPassAttachmentOutput depth = {
            .image = depthImage.getImage(),
            .imageView = depthImage.getImageView(),
            .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
    };

    return DeferredRenderPassOutput {
        .color = color,
        .normal = normal,
        .position = position,
        .depth = depth,
    };
}
