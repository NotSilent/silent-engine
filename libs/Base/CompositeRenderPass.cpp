#include "CompositeRenderPass.h"
#include "CommandBuffer.h"
#include <array>

CompositeRenderPass::CompositeRenderPass(VkDevice device, VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout, VkPipeline pipeline, VkRect2D renderArea, const DeferredRenderPassOutput& deferredRenderPassOutput)
    : descriptorSet(descriptorSet)
    , pipelineLayout(pipelineLayout)
    , pipeline(pipeline)
    , renderArea(renderArea)
    , deferredRenderPassOutput(deferredRenderPassOutput) {

    sampler = createSampler(device);

    VkDescriptorImageInfo imageInfo {
        .sampler = sampler,
        .imageView = deferredRenderPassOutput.color.imageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet descriptorWrite {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = descriptorSet,
        .dstBinding = 0,
        .dstArrayElement =0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &imageInfo,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void CompositeRenderPass::render(VkCommandBuffer cmd, VkImage swapchainImage, VkImageView swapchainImageView) {
    beginRenderPass(cmd, swapchainImage, swapchainImageView);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDraw(cmd, 6, 1, 0, 0);

    endRenderPass(cmd, swapchainImage);
}

void CompositeRenderPass::beginRenderPass(VkCommandBuffer cmd, VkImage swapchainImage, VkImageView swapchainImageView) {
    VkRenderingAttachmentInfo swapchainAttachment {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = swapchainImageView,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = nullptr,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = swapchainClearValue,
    };

    std::array attachments {swapchainAttachment};

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   swapchainImage,
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                   deferredRenderPassOutput.color.accessMask,
                                   VK_ACCESS_SHADER_READ_BIT,
                                   deferredRenderPassOutput.color.imageLayout,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   deferredRenderPassOutput.color.image,
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    VkRenderingInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = renderArea,
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = attachments.size(),
            .pColorAttachments = attachments.data(),
            .pDepthAttachment = nullptr,
            .pStencilAttachment = nullptr,
    };

    vkCmdBeginRendering(cmd, &renderingInfo);
}

void CompositeRenderPass::endRenderPass(VkCommandBuffer cmd, VkImage swapchainImage) {
    vkCmdEndRendering(cmd);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_ACCESS_NONE,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                   swapchainImage,
                                   VK_IMAGE_ASPECT_COLOR_BIT);
}

VkSampler CompositeRenderPass::createSampler(VkDevice device) {
    VkSamplerCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .mipLodBias = 0.0f,
        .anisotropyEnable = false,
        .maxAnisotropy = 0.0f,
        .compareEnable = false,
        .compareOp = VK_COMPARE_OP_NEVER,
        .minLod = 0,
        .maxLod = 0,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE,
        .unnormalizedCoordinates = false,
    };

    VkSampler sampler;
    if(vkCreateSampler(device, &createInfo, nullptr, &sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("CompositeRenderPass::createSampler");
    }

    return sampler;
}
