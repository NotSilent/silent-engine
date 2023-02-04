#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>
#include <vector>

#include "Buffer.h"
#include "Vertex.h"
#include "VertexAttribute.h"

class Mesh {
public:
    // TODO: One vertex/index, use offset for evertything
    Mesh(uint32_t indexCount, VkBuffer vertexBuffer, VkBuffer indexBuffer,
         std::vector<VertexAttribute> attributes);

    void destroy(VkDevice device, VmaAllocator allocator);

    [[nodiscard]] uint32_t getIndexCount() const;

    [[nodiscard]] VkBuffer getVertexBuffer() const;

    [[nodiscard]] VkBuffer getIndexBuffer() const;

    [[nodiscard]] std::vector<VertexAttribute> const &getAttributes() const;

private:
    uint32_t _indexCount;
    VkBuffer _vertexBuffer;
    VkBuffer _indexBuffer;

    std::vector<VertexAttribute> _attributes;
};