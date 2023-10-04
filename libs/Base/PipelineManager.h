#pragma once

#include "VkBootstrap.h"
#include "ShaderManager.h"
#include <memory>

class PipelineManager {
public:
    PipelineManager(VkDevice device, VkFormat swapchainFormat, const VkRect2D& renderArea);

    // TODO: Remove all destroys?
    void destroy();

    // TODO: Make set during creation of renderpass?
    [[nodiscard]] VkDescriptorSet getDeferredLightningSet(uint32_t frameIndex) const;

    [[nodiscard]] VkPipelineLayout getDeferredPipelineLayout() const;
    [[nodiscard]] VkPipelineLayout getDeferredLightningPipelineLayout() const;

    [[nodiscard]] VkPipeline getDeferredPipeline() const;
    [[nodiscard]] VkPipeline getDeferredLightningPipeline() const;

private:
    VkDevice device;

    VkRect2D renderArea;

    ShaderManager shaderManager;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout deferredLightningDescriptorSetLayout;
    VkDescriptorSet deferredLightningSets[3];

    VkPipelineLayout deferredPipelineLayout;
    VkPipelineLayout deferredLightningPipelineLayout;

    VkPipeline deferredPipeline;
    VkPipeline deferredLightningPipeline;

    [[nodiscard]] VkDescriptorPool createDescriptorPool();

    [[nodiscard]] VkDescriptorSetLayout createDescriptorSetLayout();

    [[nodiscard]] VkPipelineLayout createPipelineLayout(uint32_t setLayoutCount, const VkDescriptorSetLayout* pSetLayouts,
                                                        uint32_t pushConstantRangeCount, const VkPushConstantRange* pushConstantRange);

    [[nodiscard]] VkPipeline createDeferredPipeline();

    [[nodiscard]] VkPipeline createDeferredLightningPipeline(VkFormat swapchainFormat);

    // TODO: unify color attachments?
    [[nodiscard]] VkPipeline createPipeline(const Shader& shader,
                                                           uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription* pVertexBindingDescriptions,
                                                           uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription* pVertexAttributeDescriptions,
                                                           uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* pAttachments,
                                                           uint32_t colorAttachmentCount, const VkFormat* pColorAttachmentFormats,
                                                           VkPipelineLayout pipelineLayout, VkCullModeFlags cullMode);

    [[nodiscard]] static VkPipelineVertexInputStateCreateInfo
    createPipelineVertexInputStateCreateInfo(uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription* pVertexBindingDescriptions,
                                             uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription* pVertexAttributeDescriptions);

    [[nodiscard]] static VkPipelineShaderStageCreateInfo
    createPipelineShaderStageCreateInfo(VkShaderStageFlagBits shaderStage, VkShaderModule module);

    [[nodiscard]] static VkPipelineRasterizationStateCreateInfo
    createRasterizationStateCreateInfo(VkCullModeFlags cullMode);

    [[nodiscard]] static VkPipelineInputAssemblyStateCreateInfo
    createPipelineInputAssemblyStateCreateInfo();

    [[nodiscard]] static VkPipelineTessellationStateCreateInfo
    createPipelineTessellationStateCreateInfo();

    [[nodiscard]] static VkPipelineViewportStateCreateInfo
    createViewportStateCreateInfo(const VkViewport& viewport, const VkRect2D& renderArea);

    [[nodiscard]] static VkPipelineMultisampleStateCreateInfo
    createPipelineMultisampleStateCreateInfo();

    [[nodiscard]] static VkPipelineDepthStencilStateCreateInfo
    createPipelineDepthStencilStateCreateInfo();

    [[nodiscard]] static VkPipelineColorBlendAttachmentState
    createPipelineColorBlendAttachmentState();

    [[nodiscard]] static VkPipelineColorBlendStateCreateInfo
    createPipelineColorBlendStateCreateInfo(uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* pAttachments);

    [[nodiscard]] static VkPipelineDynamicStateCreateInfo
    createPipelineDynamicStateCreateInfo();

    // TODO: configure depth
    [[nodiscard]] static VkPipelineRenderingCreateInfoKHR
    createPipelineRenderingCreateInfoKHR(uint32_t colorAttachmentCount, const VkFormat* pColorAttachmentFormats);

    VkDescriptorSet createDeferredLightningSet();
};

// https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#descriptorsets-compatibility
// Two pipeline layouts are defined to be “compatible for push constants”
// if they were created with identical push constant ranges.
// Two pipeline layouts are defined to be “compatible for set N”
// if they were created with identically defined descriptor set layouts for sets zero through N,
// if both of them either were or were not created with VK_PIPELINE_LAYOUT_CREATE_INDEPENDENT_SETS_BIT_EXT,
// and if they were created with identical push constant ranges.

// Abstract descriptors
// 0 -> Per Frame
// 1 -> Per RenderPass
// 2 -> Per Material
// 3 -> Per Object