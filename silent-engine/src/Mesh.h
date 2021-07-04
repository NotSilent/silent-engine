#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vulkan/vulkan.h>

#include "Buffer.h"
#include "Texture.h"
#include "Vertex.h"

struct PushData {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 viewPosition;
};

class Mesh {
public:
    Mesh(uint32_t vertexCount, const Buffer<Vertex>& vertexBuffer, uint32_t indexCount, const Buffer<uint32_t>& indexBuffer, std::shared_ptr<Texture> texture);

    void destroy(VkDevice device, VmaAllocator allocator);

    uint32_t getVertexCount() const;

    VkBuffer getVertexBuffer() const;

    uint32_t getIndexCount() const;

    VkBuffer getIndexBuffer() const;

    std::shared_ptr<Texture> getTexture() const;

private:
    uint32_t _vertexCount;
    Buffer<Vertex> _vertexBuffer;
    uint32_t _indexCount;
    Buffer<uint32_t> _indexBuffer;
    std::shared_ptr<Texture> _texture;
};