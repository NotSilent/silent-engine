#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include <vector>

#include "Buffer.h"
#include "Vertex.h"
#include <VertexAttribute.h>

class Mesh {
public:
    Mesh(uint32_t indexCount, std::shared_ptr<Buffer> indexBuffer, std::vector<VertexAttribute>& attributes);
    void destroy(VkDevice device, VmaAllocator allocator);
    uint32_t getIndexCount() const;
    VkBuffer getIndexBuffer() const;
    std::vector<VertexAttribute> getAttributes() const;

private:
    uint32_t _indexCount;
    std::shared_ptr<Buffer> _indexBuffer;

    std::vector<VertexAttribute> _attributes;
};