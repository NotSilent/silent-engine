#include <stdexcept>
#include <array>
#include "FrameData.h"
#include "VertexAttribute.h"
#include "Texture.h"
#include "PipelineManager.h"
#include "DescriptorSetManager.h"


FrameData::FrameData(VkDevice device, VmaAllocator allocator, uint32_t width, uint32_t height,
                     VkRenderPass compatibleRenderPass, VkImageView swapchainImageView,
                     const std::shared_ptr<PipelineManager> &pipelineManager,
                     const std::shared_ptr<DescriptorSetManager> &descriptorSetManager)
        : _device(device) {
    createGBuffer(_device, allocator, width, height, compatibleRenderPass, swapchainImageView, pipelineManager,
                  descriptorSetManager);
}

void FrameData::destroy(VkDevice device, VmaAllocator allocator) {
    reset();

    vkDestroyFramebuffer(device, _gBuffer, nullptr);
    _gBufferResources.color->destroy(device, allocator);
    _gBufferResources.position->destroy(device, allocator);
    _gBufferResources.normal->destroy(device, allocator);
    _gBufferResources.depth->destroy(device, allocator);
}

void FrameData::addFence(VkFence fence) {
    _fences.push_back(fence);
}

void FrameData::addSemaphore(VkSemaphore semaphore) {
    _semaphores.push_back(semaphore);
}

void FrameData::wait() {

    if (!_fences.empty()) {
        vkWaitForFences(_device, _fences.size(), _fences.data(), VK_TRUE, std::numeric_limits<uint64_t>::max());
    }
}

void FrameData::reset() {
    for (VkFence fence: _fences) {
        vkDestroyFence(_device, fence, nullptr);
    }

    for (VkSemaphore semaphore: _semaphores) {
        vkDestroySemaphore(_device, semaphore, nullptr);
    }

    for (Command &command: _commands) {
        vkFreeCommandBuffers(_device, command._pool, 1, &command._buffer);
    }

    _fences.clear();
    _semaphores.clear();
    _commands.clear();
}

void FrameData::addCommand(VkCommandPool commandPool, VkCommandBuffer commandBuffer) {
    _commands.emplace_back(Command{commandPool, commandBuffer});
}

void FrameData::createGBuffer(VkDevice device, VmaAllocator allocator, uint32_t width, uint32_t height,
                              VkRenderPass renderPass, VkImageView swapchainImageView,
                              const std::shared_ptr<PipelineManager> &pipelineManager,
                              const std::shared_ptr<DescriptorSetManager> &descriptorSetManager) {
    ImageCreateInfo colorImageInfo{
            .extent = {width, height, 1},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };
    _gBufferResources.color = std::make_shared<Image>(device, allocator, colorImageInfo);

    ImageCreateInfo positionImageInfo{
            .extent = {width, height, 1},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_R16G16B16A16_SFLOAT,
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };
    _gBufferResources.position = std::make_shared<Image>(device, allocator, positionImageInfo);

    ImageCreateInfo normalImageInfo{
            .extent = {width, height, 1},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_R16G16B16A16_SFLOAT,
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };
    _gBufferResources.normal = std::make_shared<Image>(device, allocator, normalImageInfo);

    ImageCreateInfo depthImageInfo{
            .extent = {width, height, 1},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_D32_SFLOAT_S8_UINT,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
    };
    _gBufferResources.depth = std::make_shared<Image>(device, allocator, depthImageInfo);

    std::array<VkImageView, 5> attachments{
            swapchainImageView,
            _gBufferResources.color->getImageView(),
            _gBufferResources.position->getImageView(),
            _gBufferResources.normal->getImageView(),
            _gBufferResources.depth->getImageView(),
    };

    VkFramebufferCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .renderPass = renderPass,
            .attachmentCount = attachments.size(),
            .pAttachments = attachments.data(),
            .width = width,
            .height = height,
            .layers = 1,
    };

    if (vkCreateFramebuffer(device, &createInfo, nullptr, &_gBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create GBuffer");
    }

    const std::vector<VertexAttributeDescription> compositeVertexAttributeDescriptions;//{
//            {
//                    .type = VertexAttributeType::Position,
//                    .format = VK_FORMAT_R16_SFLOAT,
//                    .stride = 0
//            },
//    };

    const std::vector<VkDescriptorType> compositeDescriptorTypes{
            VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
    };

    std::vector<std::shared_ptr<Texture>> textures;
    textures.push_back(std::make_shared<Texture>(std::make_shared<Sampler>(), _gBufferResources.color));
    textures.push_back(std::make_shared<Texture>(std::make_shared<Sampler>(), _gBufferResources.position));
    textures.push_back(std::make_shared<Texture>(std::make_shared<Sampler>(), _gBufferResources.normal));

    _compositePipeline = pipelineManager->getPipeline(compositeVertexAttributeDescriptions, compositeDescriptorTypes,
                                                      "composite", 1);
    _compositeDescriptorSet = descriptorSetManager->getDescriptorSet(compositeDescriptorTypes, textures);
}

VkFramebuffer FrameData::getGBuffer() const {
    return _gBuffer;
}