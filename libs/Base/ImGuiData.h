#pragma once

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "GLFW/glfw3.h"
#include <functional>
#include "glm/vec3.hpp"
#include "glm/gtx/string_cast.hpp"
#include <vulkan/vulkan.h>
#include <stdexcept>

#include "VkResource.h"
#include "vk_mem_alloc.h"
#include "SimpleIni.h"
#include "nfd.hpp"

struct ImGuiFrameData {
    uint64_t currentFrame;
    float currentTime;
    float frameTime;
    float fps;
};

class ImGuiData : public VkResource<ImGuiData> {
public:
    ImGuiData() = default;

    ImGuiData(GLFWwindow *window, const VkInstance instance, const VkPhysicalDevice physicalDevice,
              const VkDevice device,
              uint32_t queueFamily, VkQueue graphicsQueue, uint32_t imageCount, VkRenderPass renderPass,
              VkCommandPool commandPool)
            : _device(device) {
        return;
        _descriptorPool = createDescriptorPool(device);

        ImGui_ImplVulkan_InitInfo initInfo{
                .Instance = instance,
                .PhysicalDevice = physicalDevice,
                .Device = device,
                .QueueFamily = queueFamily,
                .Queue = graphicsQueue,
                .PipelineCache = VK_NULL_HANDLE,
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
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    }

    void destroy(VkDevice device, VmaAllocator allocator) {
        vkDestroyDescriptorPool(device, _descriptorPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
    }

    void render(bool drawEditor, std::function<void(const std::string &)> onFileSelected) {
        return;
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!drawEditor) {
            bool shouldRenderFPSCounter = true;
            ImGui::Begin("Frame Data", &shouldRenderFPSCounter,
                         ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMouseInputs);
            ImGui::Text("Current Frame: %.llu", _frameData.currentFrame);
            ImGui::Text("Current Time:  %.2f s", _frameData.currentTime);
            ImGui::Text("Frame Time:    %.4f ms", _frameData.frameTime);
            ImGui::Text("FPS:           %.2f", _frameData.fps);
            ImGui::Text("", _frameData.fps);
            ImGui::Text("Camera position: %s", glm::to_string(_cameraPosition).c_str());
            ImGui::End();
        }

        // TODO: Build better config class
        if (!drawEditor) {
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Open", "Ctrl+O")) {

                        CSimpleIniA ini;
                        SI_Error rc = ini.LoadFile("sengine.ini");
                        if (rc == SI_OK) {
                            std::string defaultFilePathName = ".";
                            defaultFilePathName = ini.GetValue("DEFAULT", "FilePathName", defaultFilePathName.c_str());

                            NFD_Init();
                            nfdchar_t *filePath;
                            nfdfilteritem_t filterItem{"GLTF", "gltf"};
                            nfdresult_t result = NFD::OpenDialog(filePath, &filterItem, 1, defaultFilePathName.c_str());
                            if (result == NFD_OKAY) {
                                ini.SetValue("DEFAULT", "FilePathName", filePath);
                                ini.SaveFile("sengine.ini");

                                onFileSelected(filePath);
                                NFD::FreePath(filePath);
                            }

                            NFD::Quit();
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }

        ImGui::Render();
    }

    static void appendDrawToCommandBuffer(VkCommandBuffer &cmd) {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    }

    void setCameraPosition(const glm::vec3 &position) {
        _cameraPosition = position;
    }

    void setFrameData(uint64_t currentFrame, float currentTime, float frameTime, float fps) {
        _frameData = {currentFrame, currentTime, frameTime, fps};
    }

private:
    VkDevice _device;
    VkDescriptorPool _descriptorPool;

    ImGuiFrameData _frameData;
    glm::vec3 _cameraPosition;

    static VkDescriptorPool createDescriptorPool(const VkDevice device) {
        VkDescriptorPoolSize descriptorPoolSizes[]{
                {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
        };

        VkDescriptorPoolCreateInfo createInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .pNext = nullptr,
                .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                .maxSets = 1000,
                .poolSizeCount = static_cast<uint32_t>(std::size(descriptorPoolSizes)),
                .pPoolSizes = descriptorPoolSizes,
        };

        VkDescriptorPool descriptorPool;
        if (vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkCreateDescriptorPool");
        }

        return descriptorPool;
    }

    // Based on vulkan-tutorial
    VkCommandBuffer beginSingleTimeCommands(const VkCommandPool commandPool) {
        VkCommandBufferAllocateInfo allocateInfo{
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

        VkCommandBufferBeginInfo beginInfo{
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
    void endSingleTimeCommands(const VkCommandBuffer commandBuffer, const VkQueue graphicsQueue,
                               const VkCommandPool commandPool) {
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Error: vkEndCommandBuffer");
        }

        VkSubmitInfo submitInfo{
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