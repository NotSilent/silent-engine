#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <vector>

#include "Buffer.h"

class Mesh {
public:
    Mesh(uint32_t indexCount, vk::Buffer indexBuffer, vk::Buffer vertexBuffer, vk::Buffer attributesBuffer);

    [[nodiscard]] uint32_t getIndexCount() const;

    [[nodiscard]] vk::Buffer getIndexBuffer() const;

    [[nodiscard]] vk::Buffer getPositionsBuffer() const;

    [[nodiscard]] vk::Buffer getAttributesBuffer() const;

private:
    uint32_t indexCount;
    vk::Buffer indexBuffer;
    vk::Buffer positionsBuffer;
    vk::Buffer attributesBuffer;
};