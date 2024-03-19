#include "PipelineManager.h"
#include "DeferredRenderpass.h"
#include <algorithm>
#include <PushData.h>
#include <array>

PipelineManager::PipelineManager(vk::Device device, vk::Format swapchainFormat, const vk::Rect2D &renderArea)
        : device(device), renderArea(renderArea), shaderManager(device) {

    vk::PushConstantRange deferredPushConstantRange(
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(PushData)
    );

    struct LightPushData {
        glm::mat4 lightSpace;
        glm::vec3 view;
    };

    vk::PushConstantRange deferredLightningPushConstantRange{
            vk::ShaderStageFlagBits::eFragment,
            0,
            sizeof(LightPushData)
    };

    defaultSampler = createDefaultSampler(device);

    descriptorPool = createDescriptorPool();
    deferredLightningDescriptorSetLayout = createDescriptorSetLayout();
    deferredPipelineLayout = createPipelineLayout(0, nullptr, 1, &deferredPushConstantRange);
    deferredLightningPipelineLayout = createPipelineLayout(1, &deferredLightningDescriptorSetLayout, 1,
                                                           &deferredLightningPushConstantRange);
    deferredPipeline = createDeferredPipeline();
    deferredLightningPipeline = createDeferredLightningPipeline(swapchainFormat);

    // TODO: shadowmap pushrange/uniform
    shadowMapMaterial.layout = createPipelineLayout(0, nullptr, 1, &deferredPushConstantRange);
    static const uint32_t SHADOW_MAP_RESOLUTION = 2048;
    static const vk::Rect2D SHADOW_MAP_DIMENSIONS(
            {0, 0},
            {SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION}
    );
    shadowMapMaterial.pipeline = createShadowMapPipeline(SHADOW_MAP_DIMENSIONS);
}

void PipelineManager::destroy() {
    device.destroyPipeline(shadowMapMaterial.pipeline);
    device.destroyPipelineLayout(shadowMapMaterial.layout);

    device.destroyPipeline(deferredPipeline);
    device.destroyPipeline(deferredLightningPipeline);
    device.destroyPipelineLayout(deferredPipelineLayout);
    device.destroyPipelineLayout(deferredLightningPipelineLayout);
    device.destroyDescriptorSetLayout(deferredLightningDescriptorSetLayout);
    device.destroyDescriptorPool(descriptorPool);
    device.destroySampler(defaultSampler);

    shaderManager.destroy();
}

vk::Sampler PipelineManager::createDefaultSampler(vk::Device device) {
    vk::SamplerCreateInfo createInfo(
            {},
            vk::Filter::eLinear,
            vk::Filter::eLinear,
            vk::SamplerMipmapMode::eLinear,
            vk::SamplerAddressMode::eClampToEdge,
            vk::SamplerAddressMode::eClampToEdge,
            vk::SamplerAddressMode::eClampToEdge,
            0.0f,
            false,
            0.0f,
            false,
            vk::CompareOp::eNever,
            0,
            0,
            vk::BorderColor::eIntOpaqueWhite,
            false
    );

    vk::Sampler sampler;
    if (device.createSampler(&createInfo, nullptr, &sampler) != vk::Result::eSuccess) {
        throw std::runtime_error("DeferredLightningRenderpass::createSampler");
    }

    return sampler;
}

vk::PipelineLayout PipelineManager::getDeferredPipelineLayout() const {
    return deferredPipelineLayout;
}

vk::Pipeline PipelineManager::getDeferredPipeline() const {
    return deferredPipeline;
}

DeferredLightningMaterial
PipelineManager::createDeferredLightningMaterial(vk::ImageView color, vk::ImageView normal, vk::ImageView position,
                                                 vk::ImageView shadowMap) {
    std::array imageInfos{
            vk::DescriptorImageInfo(
                    defaultSampler,
                    color,
                    vk::ImageLayout::eShaderReadOnlyOptimal
            ),
            vk::DescriptorImageInfo(
                    defaultSampler,
                    normal,
                    vk::ImageLayout::eShaderReadOnlyOptimal
            ),
            vk::DescriptorImageInfo(
                    defaultSampler,
                    position,
                    vk::ImageLayout::eShaderReadOnlyOptimal
            ),
            vk::DescriptorImageInfo(
                    defaultSampler,
                    shadowMap,
                    vk::ImageLayout::eShaderReadOnlyOptimal
            )
    };

    vk::DescriptorSet set = createDeferredLightningSet();
    deferredLightningSets.push_back(set);

    vk::WriteDescriptorSet descriptorWrite(
            set,
            0,
            0,
            imageInfos.size(),
            vk::DescriptorType::eCombinedImageSampler,
            imageInfos.data(),
            nullptr,
            nullptr
    );

    device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);

    return {
            .layout = deferredLightningPipelineLayout,
            .pipeline = deferredLightningPipeline,
            .set = set,
    };
}

ShadowMapMaterial PipelineManager::getShadowMapMaterial() const {
    return shadowMapMaterial;
}


vk::DescriptorPool PipelineManager::createDescriptorPool() {
    // TODO: Configurable and per type
    // Currently should == swapchain images
    static uint32_t DescriptorSetCount = 4;

    std::array descriptorPoolSizes{
            vk::DescriptorPoolSize(
                    vk::DescriptorType::eCombinedImageSampler,
                    DescriptorSetCount
            )
    };

    vk::DescriptorPoolCreateInfo createInfo(
            {},
            DescriptorSetCount,
            descriptorPoolSizes.size(),
            descriptorPoolSizes.data()
    );

    vk::DescriptorPool descriptorPool;
    if (device.createDescriptorPool(&createInfo, nullptr, &descriptorPool) != vk::Result::eSuccess) {
        throw std::runtime_error("PipelineManager::createDescriptorPool");
    }

    return descriptorPool;
}

vk::DescriptorSetLayout PipelineManager::createDescriptorSetLayout() {
    // TODO: try immutable samplers for deferred lightning
    std::array bindings{
            vk::DescriptorSetLayoutBinding(
                    0,
                    vk::DescriptorType::eCombinedImageSampler,
                    1,
                    vk::ShaderStageFlagBits::eFragment,
                    nullptr
            ),
            vk::DescriptorSetLayoutBinding(
                    1,
                    vk::DescriptorType::eCombinedImageSampler,
                    1,
                    vk::ShaderStageFlagBits::eFragment,
                    nullptr
            ),
            vk::DescriptorSetLayoutBinding(
                    2,
                    vk::DescriptorType::eCombinedImageSampler,
                    1,
                    vk::ShaderStageFlagBits::eFragment,
                    nullptr
            ),
            vk::DescriptorSetLayoutBinding(
                    3,
                    vk::DescriptorType::eCombinedImageSampler,
                    1,
                    vk::ShaderStageFlagBits::eFragment,
                    nullptr
            )
    };

    vk::DescriptorSetLayoutCreateInfo createInfo(
            {},
            bindings.size(),
            bindings.data()
    );

    vk::DescriptorSetLayout descriptorSetLayout;
    if (device.createDescriptorSetLayout(&createInfo, nullptr, &descriptorSetLayout) != vk::Result::eSuccess) {
        throw std::runtime_error("PipelineManager::vk::CreateDescriptorSetLayout");
    }

    return descriptorSetLayout;
}

vk::DescriptorSet PipelineManager::createDeferredLightningSet() {
    vk::DescriptorSetAllocateInfo allocateInfo(
            descriptorPool,
            1,
            &deferredLightningDescriptorSetLayout
    );

    vk::DescriptorSet descriptorSet;
    if (device.allocateDescriptorSets(&allocateInfo, &descriptorSet) != vk::Result::eSuccess) {
        throw std::runtime_error("PipelineManager::createDeferredLightningSet");
    }

    return descriptorSet;
}

vk::PipelineLayout
PipelineManager::createPipelineLayout(uint32_t setLayoutCount, const vk::DescriptorSetLayout *pSetLayouts,
                                      uint32_t pushConstantRangeCount, const vk::PushConstantRange *pushConstantRange) {
    const vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
            {},
            setLayoutCount,
            pSetLayouts,
            pushConstantRangeCount,
            pushConstantRange
    );

    vk::PipelineLayout pipeline;
    if (device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipeline) != vk::Result::eSuccess) {
        throw std::runtime_error("PipelineManager::createPipelineLayout");
    }

    return pipeline;
}

vk::Pipeline PipelineManager::createDeferredPipeline() {
    std::optional<Shader> shader = shaderManager.getShader("Deferred");
    if (shader.has_value()) {
        std::array vertexBindingDescriptions = {
                vk::VertexInputBindingDescription(0, sizeof(glm::vec3), vk::VertexInputRate::eVertex),
                vk::VertexInputBindingDescription(1, sizeof(glm::vec3), vk::VertexInputRate::eVertex)
        };

        std::array vertexInputAttributeDescription{
                vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0),
                vk::VertexInputAttributeDescription(1, 1, vk::Format::eR32G32B32Sfloat, 0),
        };

        static const size_t ATTACHMENT_COUNT = 3;

        std::array<vk::PipelineColorBlendAttachmentState, ATTACHMENT_COUNT> colorBlendAttachments{
                createPipelineColorBlendAttachmentState(),
                createPipelineColorBlendAttachmentState(),
                createPipelineColorBlendAttachmentState(),
        };

        // TODO: manage format
        std::array<vk::Format, ATTACHMENT_COUNT> colorAttachmentFormats{
                DeferredRenderpassDefinitions::Formats::COLOR,
                DeferredRenderpassDefinitions::Formats::NORMAL,
                DeferredRenderpassDefinitions::Formats::POSITION,
        };

        return createPipeline(renderArea, shader.value(),
                              vertexBindingDescriptions.size(),
                              vertexBindingDescriptions.data(),
                              vertexInputAttributeDescription.size(),
                              vertexInputAttributeDescription.data(),
                              colorBlendAttachments.size(),
                              colorBlendAttachments.data(),
                              colorAttachmentFormats.size(),
                              colorAttachmentFormats.data(),
                              deferredPipelineLayout,
                              vk::CullModeFlagBits::eBack);
    }

    return {};
}

vk::Pipeline PipelineManager::createDeferredLightningPipeline(vk::Format swapchainFormat) {
    std::optional<Shader> shader = shaderManager.getShader("DeferredLightning");
    if (shader.has_value()) {
        std::array<vk::PipelineColorBlendAttachmentState, 1> colorBlendAttachments{
                createPipelineColorBlendAttachmentState(),
        };

        std::array<vk::Format, 1> colorAttachmentFormats{swapchainFormat};

        return createPipeline(renderArea, shader.value(),
                              0,
                              nullptr,
                              0,
                              nullptr,
                              colorBlendAttachments.size(),
                              colorBlendAttachments.data(),
                              colorAttachmentFormats.size(),
                              colorAttachmentFormats.data(),
                              deferredLightningPipelineLayout,
                              vk::CullModeFlagBits::eBack);
    }

    return {};
}

vk::Pipeline PipelineManager::createShadowMapPipeline(const vk::Rect2D &renderArea) {
    std::optional<Shader> shader = shaderManager.getShader("ShadowMap");
    if (shader.has_value()) {
        std::array vertexBindingDescriptions = {
                vk::VertexInputBindingDescription(0, sizeof(glm::vec3), vk::VertexInputRate::eVertex)
        };

        std::array vertexInputAttributeDescription{
                vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0)
        };

        static const size_t ATTACHMENT_COUNT = 0;

        std::array<vk::PipelineColorBlendAttachmentState, ATTACHMENT_COUNT> colorBlendAttachments{
                //createPipelineColorBlendAttachmentState(),
        };

        // TODO: manage format
        std::array<vk::Format, ATTACHMENT_COUNT> colorAttachmentFormats{
                // TODO: ShadowMapDefinitions
                //DeferredRenderpassDefinitions::Formats::DEPTH,
        };

        return createPipeline(renderArea, shader.value(),
                              vertexBindingDescriptions.size(),
                              vertexBindingDescriptions.data(),
                              vertexInputAttributeDescription.size(),
                              vertexInputAttributeDescription.data(),
                              colorBlendAttachments.size(),
                              colorBlendAttachments.data(),
                              colorAttachmentFormats.size(),
                              colorAttachmentFormats.data(),
                              shadowMapMaterial.layout,
                              vk::CullModeFlagBits::eBack);
    }

    return {};
}

vk::Pipeline PipelineManager::createPipeline(const vk::Rect2D &renderArea, const Shader &shader,
                                             uint32_t vertexBindingDescriptionCount,
                                             const vk::VertexInputBindingDescription *pVertexBindingDescriptions,
                                             uint32_t vertexAttributeDescriptionCount,
                                             const vk::VertexInputAttributeDescription *pVertexAttributeDescriptions,
                                             uint32_t attachmentCount,
                                             const vk::PipelineColorBlendAttachmentState *pAttachments,
                                             uint32_t colorAttachmentCount, const vk::Format *pColorAttachmentFormats,
                                             vk::PipelineLayout pipelineLayout, vk::CullModeFlags cullMode) {
    const std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageCreateInfos = {
            createPipelineShaderStageCreateInfo(vk::ShaderStageFlagBits::eVertex, shader.vert),
            createPipelineShaderStageCreateInfo(vk::ShaderStageFlagBits::eFragment, shader.frag),
    };

    const vk::PipelineVertexInputStateCreateInfo vertexInputState = createPipelineVertexInputStateCreateInfo(
            vertexBindingDescriptionCount, pVertexBindingDescriptions, vertexAttributeDescriptionCount,
            pVertexAttributeDescriptions);

    const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState = createPipelineInputAssemblyStateCreateInfo();
    const vk::PipelineTessellationStateCreateInfo tessellationState = createPipelineTessellationStateCreateInfo();

    const vk::Viewport viewport(
            0.0f,
            0.0f,
            static_cast<float>(renderArea.extent.width),
            static_cast<float>(renderArea.extent.height),
            0.0f,
            1.0f
    );

    const vk::PipelineViewportStateCreateInfo viewportState = createViewportStateCreateInfo(viewport, renderArea);
    const vk::PipelineRasterizationStateCreateInfo rasterizationState = createRasterizationStateCreateInfo(cullMode);
    const vk::PipelineMultisampleStateCreateInfo multisampleState = createPipelineMultisampleStateCreateInfo();
    const vk::PipelineDepthStencilStateCreateInfo depthStencilState = createPipelineDepthStencilStateCreateInfo();
    const vk::PipelineColorBlendStateCreateInfo colorBlendState = createPipelineColorBlendStateCreateInfo(
            attachmentCount,
            pAttachments);
    const vk::PipelineDynamicStateCreateInfo dynamicState = createPipelineDynamicStateCreateInfo();
    const vk::PipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = createPipelineRenderingCreateInfoKHR(
            colorAttachmentCount, pColorAttachmentFormats);

    vk::GraphicsPipelineCreateInfo createInfo(
            {},
            shaderStageCreateInfos.size(),
            shaderStageCreateInfos.data(),
            &vertexInputState,
            &inputAssemblyState,
            &tessellationState,
            &viewportState,
            &rasterizationState,
            &multisampleState,
            &depthStencilState,
            &colorBlendState,
            &dynamicState,
            pipelineLayout,
            nullptr,
            0,
            nullptr,
            0
    );

    createInfo.setPNext(&pipelineRenderingCreateInfo);

    vk::Pipeline pipeline;
    if (device.createGraphicsPipelines(nullptr, 1, &createInfo, nullptr, &pipeline) != vk::Result::eSuccess) {
        throw std::runtime_error("PipelineManager::createPipeline");
    }

    return pipeline;
}

vk::PipelineShaderStageCreateInfo
PipelineManager::createPipelineShaderStageCreateInfo(vk::ShaderStageFlagBits shaderStage, vk::ShaderModule module) {
    return {{}, shaderStage, module, "main", nullptr};
}

vk::PipelineVertexInputStateCreateInfo
PipelineManager::createPipelineVertexInputStateCreateInfo(uint32_t vertexBindingDescriptionCount,
                                                          const vk::VertexInputBindingDescription *pVertexBindingDescriptions,
                                                          uint32_t vertexAttributeDescriptionCount,
                                                          const vk::VertexInputAttributeDescription *pVertexAttributeDescriptions) {
    return vk::PipelineVertexInputStateCreateInfo{
            {},
            vertexBindingDescriptionCount,
            pVertexBindingDescriptions,
            vertexAttributeDescriptionCount,
            pVertexAttributeDescriptions
    };
}

vk::PipelineRasterizationStateCreateInfo
PipelineManager::createRasterizationStateCreateInfo(vk::CullModeFlags cullMode) {
    return vk::PipelineRasterizationStateCreateInfo{
            {},
            false,
            false,
            vk::PolygonMode::eFill,
            cullMode,
            vk::FrontFace::eCounterClockwise,
            false,
            0.0f,
            0.0f,
            0.0f,
            1.0f
    };
}

vk::PipelineInputAssemblyStateCreateInfo PipelineManager::createPipelineInputAssemblyStateCreateInfo() {
    return vk::PipelineInputAssemblyStateCreateInfo{{}, vk::PrimitiveTopology::eTriangleList, false};
}

vk::PipelineTessellationStateCreateInfo PipelineManager::createPipelineTessellationStateCreateInfo() {
    return vk::PipelineTessellationStateCreateInfo{{}, 0};
}

vk::PipelineViewportStateCreateInfo
PipelineManager::createViewportStateCreateInfo(const vk::Viewport &viewport, const vk::Rect2D &renderArea) {
    return vk::PipelineViewportStateCreateInfo{{}, 1, &viewport, 1, &renderArea};
}

vk::PipelineMultisampleStateCreateInfo PipelineManager::createPipelineMultisampleStateCreateInfo() {
    return vk::PipelineMultisampleStateCreateInfo{
            {},
            vk::SampleCountFlagBits::e1,
            vk::False,
            0.0f,
            nullptr,
            vk::False,
            vk::False,
    };
}

vk::PipelineDepthStencilStateCreateInfo PipelineManager::createPipelineDepthStencilStateCreateInfo() {
    return vk::PipelineDepthStencilStateCreateInfo{
            {},
            true,
            true,
            vk::CompareOp::eLessOrEqual,
            true,
            true,
            {},
            {},
            0.0f,
            1.0f
    };
}

vk::PipelineColorBlendAttachmentState PipelineManager::createPipelineColorBlendAttachmentState() {
    return vk::PipelineColorBlendAttachmentState{
            vk::False,
            {},
            {},
            {},
            {},
            {},
            {},
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA
    };
}

vk::PipelineColorBlendStateCreateInfo PipelineManager::createPipelineColorBlendStateCreateInfo(uint32_t attachmentCount,
                                                                                               const vk::PipelineColorBlendAttachmentState *pAttachments) {
    return vk::PipelineColorBlendStateCreateInfo{
        {},
        false,
        {},
        attachmentCount,
        pAttachments,
        {}
    };
}

vk::PipelineDynamicStateCreateInfo PipelineManager::createPipelineDynamicStateCreateInfo() {
    return vk::PipelineDynamicStateCreateInfo{{}, 0, nullptr};
}

vk::PipelineRenderingCreateInfoKHR PipelineManager::createPipelineRenderingCreateInfoKHR(uint32_t colorAttachmentCount,
                                                                                         const vk::Format *pColorAttachmentFormats) {
    return vk::PipelineRenderingCreateInfoKHR{
        0,
        colorAttachmentCount,
        pColorAttachmentFormats,
        DeferredRenderpassDefinitions::Formats::DEPTH,
        vk::Format::eUndefined,
    };
}
