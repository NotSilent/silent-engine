#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

struct ImGuiFrameData {
    uint64_t currentFrame;
    double currentTime;
    double frameTime;
    double fps;
};

class ImGuiData {
public:
    ImGuiData(GLFWwindow* window, const VkInstance instance, const VkPhysicalDevice physicalDevice, const VkDevice device,
        uint32_t queueFamily, VkQueue graphicsQueue, uint32_t imageCount, VkRenderPass renderPass, VkCommandPool commandPool)
        : _device(device)
    {
        _descriptorPool = createDescriptorPool(device);

        ImGui_ImplVulkan_InitInfo initInfo {
            .Instance = instance,
            .PhysicalDevice = physicalDevice,
            .Device = device,
            .QueueFamily = queueFamily,
            .Queue = graphicsQueue,
            .PipelineCache = nullptr,
            .DescriptorPool = _descriptorPool,
            .Subpass = 0,
            .MinImageCount = imageCount,
            .ImageCount = imageCount,
            .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
            .Allocator = nullptr,
            .CheckVkResultFn = nullptr,
        };

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(window, true);
        ImGui_ImplVulkan_Init(&initInfo, renderPass);
        ImGui::StyleColorsDark();

        auto cmd = beginSingleTimeCommands(commandPool);

        ImGui_ImplVulkan_CreateFontsTexture(cmd);

        endSingleTimeCommands(cmd, graphicsQueue, commandPool);
    }

    ~ImGuiData()
    {
        vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
    }

    void render()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool frameData { true };
        ImGui::Begin("Frame Data", &frameData, 1 << 6);
        ImGui::Text("Current Frame: %.i", _frameData.currentFrame);
        ImGui::Text("Current Time:  %.2f s", _frameData.currentTime);
        ImGui::Text("Frame Time:    %.4f ms", _frameData.frameTime);
        ImGui::Text("FPS:           %.2f", _frameData.fps);
        ImGui::End();

        ImGui::Render();
    }

    void appendDrawToCommandBuffer(VkCommandBuffer& cmd)
    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    }

    void setFrameData(uint64_t currentFrame, double currentTime, double frameTime, double fps)
    {
        _frameData = { currentFrame, currentTime, frameTime, fps };
    }

private:
    VkDevice _device;
    VkDescriptorPool _descriptorPool;

    ImGuiFrameData _frameData;

    VkDescriptorPool createDescriptorPool(const VkDevice device)
    {
        VkDescriptorPoolSize descriptorPoolSizes[] {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(descriptorPoolSizes);

        VkDescriptorPoolCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            .maxSets = 1000,
            .poolSizeCount = std::size(descriptorPoolSizes),
            .pPoolSizes = descriptorPoolSizes,
        };

        VkDescriptorPool descriptorPool;
        if (vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkCreateDescriptorPool");
        }

        return descriptorPool;
    }

    // Based on vulkan-tutorial
    VkCommandBuffer beginSingleTimeCommands(const VkCommandPool commandPool)
    {
        VkCommandBufferAllocateInfo allocateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        VkCommandBuffer commandBuffer;
        if (vkAllocateCommandBuffers(_device, &allocateInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkAllocateCommandBuffers");
        }

        VkCommandBufferBeginInfo beginInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
        };

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkBeginCommandBuffer");
        }

        return commandBuffer;
    }

    // Based on vulkan-tutorial
    void endSingleTimeCommands(const VkCommandBuffer commandBuffer, const VkQueue graphicsQueue, const VkCommandPool commandPool)
    {
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkEndCommandBuffer");
        }

        VkSubmitInfo submitInfo {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = {},
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
        };

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkQueueSubmit");
        }

        if (vkQueueWaitIdle(graphicsQueue) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkQueueWaitIdle");
        }

        vkFreeCommandBuffers(_device, commandPool, 1, &commandBuffer);
    }
};