#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "vk-bootstrap/VkBootstrap.h"

#include "VkDraw.h"
#include "VkInit.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define VMA_IMPLEMENTATION
#include "ImGuiData.h"
#include "Mesh.h"
#include "memory"
#include "vma/vk_mem_alloc.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

#include "Image.h"

// TODO: VkCommandPool and VkCommands creation manager;

const std::string ENGINE_NAME = "Silent Engine";
const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

uint64_t _currentFrame = 0;
double _currentTime { 0.0 };

vkb::Instance _instance;
vkb::PhysicalDevice _physicalDevice;
vkb::Device _device;
vkb::Swapchain _swapchain;

VkSurfaceKHR _surface;

std::vector<VkImage> _swapchainImages;
std::vector<VkImageView> _swapchainImageViews;

std::vector<std::unique_ptr<Image>> _depthStencilImages;

std::vector<VkFramebuffer> _framebuffers;

VkCommandPool _commandPool;
VkRenderPass _renderPass;

VkDescriptorPool _descriptorPool;
VkDescriptorSetLayout _defaultDescriptorSetLayout;

VkPipelineLayout _defaultPipelineLayout;

VmaAllocator _allocator;

std::unique_ptr<ImGuiData> _imGuiData;

std::unique_ptr<Mesh> _mesh;

class Camera {
public:
private:
    const glm::vec3 FORWARD { 0.0f, 0.0f, 1.0f };

    glm::vec3 _position { 0.0f, 0.0f, -1.0f };
    glm::mat4 _projection { glm::perspective(glm::radians(45.0f), WIDTH / static_cast<float>(HEIGHT), 0.0001f, 200.0f) };

    float _unitsPerSecond { 1.0f };
    float _angle{0.15};
    float _horizontalAngle { 0.0f };
    float _verticalAngle { 0.0f };

    glm::vec3 _currentDirection { 0.0f, 0.0f, 1.0f };

public:
    void update(float deltaTime, glm::vec2 directionInput, glm::vec2 rotationInput)
    {
        directionInput *= deltaTime * _unitsPerSecond;
        rotationInput *= deltaTime * _angle;

        _horizontalAngle += rotationInput.x;
        _verticalAngle = std::clamp(_verticalAngle + rotationInput.y, -90.0f + std::numeric_limits<float>::epsilon(), 90.0f + std::numeric_limits<float>::epsilon());

        _currentDirection = glm::rotateX(FORWARD, _verticalAngle);
        _currentDirection = glm::rotateY(_currentDirection, _horizontalAngle);

        glm::vec3 movementDelta = _currentDirection * directionInput.y;
        const glm::vec3 cross = glm::cross(_currentDirection, { 0.0f, 1.0f, 0.0f });
        movementDelta -= cross * directionInput.x;
        _position += movementDelta;
    }

    void setUnitsPerSecond(float value)
    {
        _unitsPerSecond = value;
    }

    void translate(const glm::vec3& translation)
    {
        _position += translation;
    }

    glm::vec3 getPosition() const
    {
        return _position;
    }

    glm::mat4 getViewMatrix() const
    {
        // TODO: up vector has negative y? NDC having y pointing down? Research to be sure
        return glm::lookAt(_position, _position + _currentDirection, { 0.0f, -1.0f, 0.0f });
    }

    glm::mat4 getProjectionMatrix() const
    {
        return _projection;
    }

private:
};

Camera _camera;

// TODO: Input manager
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        _imGuiData->toggleShow();
    }
}

void init(GLFWwindow* window);
void initMesh();
void cleanup();
void draw();

int main()
{
    if (!glfwInit()) {
        throw std::runtime_error("Error: glfwInit");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Silent Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();

        throw std::runtime_error("Error: glfwCreateWindow");
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetCursorPos(window, 0.0, 0.0);

    glfwSetKeyCallback(window, key_callback);

    init(window);
    initMesh();

    _currentTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        const double currentTime = glfwGetTime();
        // deltaTime in seconds
        const double deltaTime = currentTime - _currentTime;
        // frameTIme in milliseconds
        const double frameTime = deltaTime * 1000.0;
        const double fps = 1000.0 / frameTime;
        _currentTime = currentTime;

        glfwPollEvents();

        glm::vec2 input { 0.0f };

        // TODO: Input manager
        // TODO: Add deltaTime
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            input.y += 1.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            input.y += -1.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            input.x += -1.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            input.x += 1.0f;
        }

        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        glfwSetCursorPos(window, 0.0, 0.0);

        std::cout << xPos << " " << yPos << "\n";

        _camera.update(deltaTime, input, glm::vec2(xPos, yPos));

        _imGuiData->setFrameData(_currentFrame, currentTime, frameTime, fps);
        _imGuiData->setCameraPosition(_camera.getPosition());
        _imGuiData->render();

        draw();

        _currentFrame++;
    }

    cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void init(GLFWwindow* window)
{
    vkb::InstanceBuilder builder;
    auto instanceResult = builder.set_app_name(ENGINE_NAME.c_str()).request_validation_layers(true).use_default_debug_messenger().build();

    if (!instanceResult) {
        throw std::runtime_error(instanceResult.error().message());
    }

    _instance = instanceResult.value();

    glfwCreateWindowSurface(_instance.instance, window, nullptr, &_surface);

    vkb::PhysicalDeviceSelector selector { _instance };

    auto physicalDeviceResult = selector.set_surface(_surface).set_desired_version(1, 1).select();
    if (!physicalDeviceResult) {
        throw std::runtime_error(physicalDeviceResult.error().message());
    }

    _physicalDevice = physicalDeviceResult.value();

    vkb::DeviceBuilder deviceBuilder { _physicalDevice };

    auto deviceResult = deviceBuilder.build();
    if (!deviceResult) {
        throw std::runtime_error(deviceResult.error().message());
    }

    _device = deviceResult.value();

    vkb::SwapchainBuilder swapchainBuilder { _device };

    auto swapchainResult = swapchainBuilder.use_default_format_selection().set_desired_present_mode(VK_PRESENT_MODE_FIFO_RELAXED_KHR).set_desired_extent(WIDTH, HEIGHT).build();
    if (!swapchainResult) {
        throw std::runtime_error(swapchainResult.error().message());
    }

    _allocator = VkInit::createAllocator(_instance, _physicalDevice, _device, VK_API_VERSION_1_0, _swapchain.image_count);

    _swapchain = swapchainResult.value();
    _swapchainImages = _swapchain.get_images().value();
    _swapchainImageViews = _swapchain.get_image_views().value();

    //

    _renderPass = VkInit::createRenderPass(_device, _swapchain);

    _depthStencilImages = std::vector<std::unique_ptr<Image>>(_swapchain.image_count);
    _framebuffers = std::vector<VkFramebuffer>(_swapchain.image_count);
    for (uint32_t i = 0; i < _swapchain.image_count; ++i) {
        _depthStencilImages[i] = std::make_unique<Image>(_device, _allocator, WIDTH, HEIGHT);
        _framebuffers[i] = VkInit::createFramebuffer(_device, _renderPass, _swapchainImageViews[i], _depthStencilImages[i]->getImageView(), WIDTH, HEIGHT);
    }

    _commandPool = VkInit::createCommandPool(_device);

    _descriptorPool = VkInit::createDescriptorPool(_device);

    _defaultDescriptorSetLayout = VkInit::createDefaultDescriptorSetLayout(_device);

    _defaultPipelineLayout = VkInit::Pipeline::createPipelineLayout(_device, 1, &_defaultDescriptorSetLayout, sizeof(PushData));

    _imGuiData = std::make_unique<ImGuiData>(window, _instance.instance, _physicalDevice.physical_device, _device.device,
        _device.get_queue_index(vkb::QueueType::graphics).value(), _device.get_queue(vkb::QueueType::graphics).value(), _swapchain.image_count, _renderPass, _commandPool);
}

// TODO: move to mesh factory
void initMesh()
{
    Assimp::Importer importer {};

    const aiScene* scene = importer.ReadFile("assets/stanford-bunny.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals);

    std::cout << importer.GetErrorString();

    if (!scene || !scene->HasMeshes()) {
        throw std::runtime_error(importer.GetErrorString());
    }

    const auto assimpMesh = scene->mMeshes[0];

    std::vector<Vertex> meshData { assimpMesh->mNumVertices };
    for (uint32_t i = 0; i < assimpMesh->mNumVertices; ++i) {
        meshData[i].position.x = assimpMesh->mVertices[i].x;
        meshData[i].position.y = assimpMesh->mVertices[i].y;
        meshData[i].position.z = assimpMesh->mVertices[i].z;
        meshData[i].normal.x = assimpMesh->mNormals[i].x;
        meshData[i].normal.y = assimpMesh->mNormals[i].y;
        meshData[i].normal.z = assimpMesh->mNormals[i].z;
    }

    std::vector<uint32_t> indices(assimpMesh->mNumFaces * 3);
    for (uint32_t i = 0; i < assimpMesh->mNumFaces; ++i) {
        indices[i * 3] = assimpMesh->mFaces[i].mIndices[0];
        indices[i * 3 + 1] = assimpMesh->mFaces[i].mIndices[1];
        indices[i * 3 + 2] = assimpMesh->mFaces[i].mIndices[2];
    }

    _mesh = std::make_unique<Mesh>(_device, _allocator, _defaultPipelineLayout, _renderPass, WIDTH, HEIGHT, _commandPool, meshData, indices);
}

void cleanup()
{
    for (auto& image : _depthStencilImages) {
        image.reset();
    }
    _mesh.reset();
    _imGuiData.reset();

    vkDestroyPipelineLayout(_device.device, _defaultPipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(_device.device, _defaultDescriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(_device.device, _descriptorPool, nullptr);

    vmaDestroyAllocator(_allocator);

    for (uint32_t i = 0; i < _swapchain.image_count; ++i) {
        vkDestroyFramebuffer(_device.device, _framebuffers[i], nullptr);
    }

    vkDestroyRenderPass(_device.device, _renderPass, nullptr);

    vkDestroyCommandPool(_device.device, _commandPool, nullptr);

    _swapchain.destroy_image_views(_swapchainImageViews);
    vkb::destroy_swapchain(_swapchain);

    vkDestroySurfaceKHR(_instance.instance, _surface, nullptr);

    vkb::destroy_device(_device);
    vkb::destroy_instance(_instance);
}

void draw()
{
    auto acquireSemaphore = VkInit::createSemaphore(_device);
    uint32_t imageIndex;

    if (vkAcquireNextImageKHR(_device.device, _swapchain.swapchain, std::numeric_limits<uint32_t>::max(), acquireSemaphore, nullptr, &imageIndex) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkAcquireNextImageKHR");
    }

    VkClearValue clearValues[] {
        { 0.75f, 0.75f, 0.75f },
        { 1.0f, 0 },
    };

    PushData pushData {
        .model = glm::mat4(1.0f),
        .view = _camera.getViewMatrix(),
        .projection = _camera.getProjectionMatrix(),
        .viewPosition = _camera.getPosition(),
    };

    VkCommandBuffer cmd = VkDraw::recordCommandBuffer(_device, _commandPool, _mesh.get(), _renderPass, _framebuffers[imageIndex],
        VkRect2D { 0, 0, WIDTH, HEIGHT }, std::size(clearValues), clearValues, _imGuiData.get(), pushData);

    auto queueFence = VkInit::createFence(_device);

    VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    auto submitSemaphore = VkInit::createSemaphore(_device);

    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &acquireSemaphore,
        .pWaitDstStageMask = &waitDstStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &submitSemaphore,
    };

    vkQueueSubmit(_device.get_queue(vkb::QueueType::graphics).value(), 1, &submitInfo, queueFence);

    VkPresentInfoKHR presentInfo {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &submitSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &_swapchain.swapchain,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    vkQueuePresentKHR(_device.get_queue(vkb::QueueType::graphics).value(), &presentInfo);

    vkWaitForFences(_device.device, 1, &queueFence, true, std::numeric_limits<uint64_t>::max());

    vkFreeCommandBuffers(_device.device, _commandPool, 1, &cmd);
    vkDestroyFence(_device.device, queueFence, nullptr);
    vkDestroySemaphore(_device.device, acquireSemaphore, nullptr);
    vkDestroySemaphore(_device.device, submitSemaphore, nullptr);
}