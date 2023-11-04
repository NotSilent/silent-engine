#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>
#include <vector>

#include "Buffer.h"

class Mesh {
public:
    Mesh(uint32_t indexCount, VkBuffer indexBuffer, VkBuffer vertexBuffer, VkBuffer attributesBuffer);

    [[nodiscard]] uint32_t getIndexCount() const;

    [[nodiscard]] VkBuffer getIndexBuffer() const;

    [[nodiscard]] VkBuffer getPositionsBuffer() const;

    [[nodiscard]] VkBuffer getAttributesBuffer() const;

private:
    uint32_t indexCount;
    VkBuffer indexBuffer;
    VkBuffer positionsBuffer;
    VkBuffer attributesBuffer;
};