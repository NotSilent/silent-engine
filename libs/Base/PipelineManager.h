#pragma once

#include "VkBootstrap.h"
#include "ShaderManager.h"
#include "DeferredLightningMaterial.h"
#include "ShadowMapMaterial.h"
#include <memory>

class PipelineManager {
public:
    PipelineManager(vk::Device device, vk::Format swapchainFormat, const vk::Rect2D &renderArea);

    void destroy();

    PipelineManager(PipelineManager &other) = delete;

    PipelineManager &operator=(PipelineManager &other) = delete;

    PipelineManager(PipelineManager &&other) = default;

    PipelineManager &operator=(PipelineManager &&other) = default;

    [[nodiscard]] vk::PipelineLayout getDeferredPipelineLayout() const;

    [[nodiscard]] vk::Pipeline getDeferredPipeline() const;

    [[nodiscard]] DeferredLightningMaterial
    createDeferredLightningMaterial(vk::ImageView color, vk::ImageView normal, vk::ImageView position, vk::ImageView shadowMap);

    [[nodiscard]] ShadowMapMaterial getShadowMapMaterial() const;

private:
    vk::Device device;

    vk::Rect2D renderArea;

    ShaderManager shaderManager;

    vk::Sampler defaultSampler;

    vk::DescriptorPool descriptorPool;

    // TODO: Separate pipelines per renderpass?
    // Doesn't make much sense for shadowmaps and final composition pipeline to be here
    // since they are unchanged and only used by those renderpasses

    vk::PipelineLayout deferredPipelineLayout;
    vk::Pipeline deferredPipeline;

    vk::DescriptorSetLayout deferredLightningDescriptorSetLayout;
    vk::PipelineLayout deferredLightningPipelineLayout;
    vk::Pipeline deferredLightningPipeline;
    std::vector<vk::DescriptorSet> deferredLightningSets;

    ShadowMapMaterial shadowMapMaterial;

    [[nodiscard]] vk::Sampler createDefaultSampler(vk::Device device);

    [[nodiscard]] vk::DescriptorPool createDescriptorPool();

    [[nodiscard]] vk::DescriptorSetLayout createDescriptorSetLayout();

    [[nodiscard]] vk::PipelineLayout
    createPipelineLayout(uint32_t setLayoutCount, const vk::DescriptorSetLayout *pSetLayouts,
                         uint32_t pushConstantRangeCount, const vk::PushConstantRange *pushConstantRange);

    [[nodiscard]] vk::Pipeline createDeferredPipeline();

    [[nodiscard]] vk::Pipeline createDeferredLightningPipeline(vk::Format swapchainFormat);

    [[nodiscard]] vk::Pipeline createShadowMapPipeline(const vk::Rect2D& renderArea);

    [[nodiscard]] vk::Pipeline createPipeline(const vk::Rect2D& renderArea, const Shader &shader,
                                            uint32_t vertexBindingDescriptionCount,
                                            const vk::VertexInputBindingDescription *pVertexBindingDescriptions,
                                            uint32_t vertexAttributeDescriptionCount,
                                            const vk::VertexInputAttributeDescription *pVertexAttributeDescriptions,
                                            uint32_t attachmentCount,
                                            const vk::PipelineColorBlendAttachmentState *pAttachments,
                                            uint32_t colorAttachmentCount, const vk::Format *pColorAttachmentFormats,
                                            vk::PipelineLayout pipelineLayout, vk::CullModeFlags cullMode);

    [[nodiscard]] static vk::PipelineVertexInputStateCreateInfo
    createPipelineVertexInputStateCreateInfo(uint32_t vertexBindingDescriptionCount,
                                             const vk::VertexInputBindingDescription *pVertexBindingDescriptions,
                                             uint32_t vertexAttributeDescriptionCount,
                                             const vk::VertexInputAttributeDescription *pVertexAttributeDescriptions);

    [[nodiscard]] static vk::PipelineShaderStageCreateInfo
    createPipelineShaderStageCreateInfo(vk::ShaderStageFlagBits shaderStage, vk::ShaderModule module);

    [[nodiscard]] static vk::PipelineRasterizationStateCreateInfo
    createRasterizationStateCreateInfo(vk::CullModeFlags cullMode);

    [[nodiscard]] static vk::PipelineInputAssemblyStateCreateInfo
    createPipelineInputAssemblyStateCreateInfo();

    [[nodiscard]] static vk::PipelineTessellationStateCreateInfo
    createPipelineTessellationStateCreateInfo();

    [[nodiscard]] static vk::PipelineViewportStateCreateInfo
    createViewportStateCreateInfo(const vk::Viewport &viewport, const vk::Rect2D &renderArea);

    [[nodiscard]] static vk::PipelineMultisampleStateCreateInfo
    createPipelineMultisampleStateCreateInfo();

    [[nodiscard]] static vk::PipelineDepthStencilStateCreateInfo
    createPipelineDepthStencilStateCreateInfo();

    [[nodiscard]] static vk::PipelineColorBlendAttachmentState
    createPipelineColorBlendAttachmentState();

    [[nodiscard]] static vk::PipelineColorBlendStateCreateInfo
    createPipelineColorBlendStateCreateInfo(uint32_t attachmentCount,
                                            const vk::PipelineColorBlendAttachmentState *pAttachments);

    [[nodiscard]] static vk::PipelineDynamicStateCreateInfo
    createPipelineDynamicStateCreateInfo();

    // TODO: configure depth
    [[nodiscard]] static vk::PipelineRenderingCreateInfoKHR
    createPipelineRenderingCreateInfoKHR(uint32_t colorAttachmentCount, const vk::Format *pColorAttachmentFormats);

    vk::DescriptorSet createDeferredLightningSet();
};

// https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vk::spec.html#descriptorsets-compatibility
// Two pipeline layouts are defined to be “compatible for push constants”
// if they were created with identical push constant ranges.
// Two pipeline layouts are defined to be “compatible for set N”
// if they were created with identically defined descriptor set layouts for sets zero through N,
// if both of them either were or were not created with vk::_PIPELINE_LAYOUT_CREATE_INDEPENDENT_SETS_BIT_EXT,
// and if they were created with identical push constant ranges.

// Abstract descriptors
// 0 -> Per Frame
// 1 -> Per RenderPass
// 2 -> Per Material
// 3 -> Per Object