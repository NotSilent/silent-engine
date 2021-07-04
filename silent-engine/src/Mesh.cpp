#include "Mesh.h"

Mesh::Mesh(uint32_t vertexCount, const Buffer<Vertex>& vertexBuffer, uint32_t indexCount, const Buffer<uint32_t>& indexBuffer, std::shared_ptr<Texture> texture) :
    _vertexCount(vertexCount), _vertexBuffer(vertexBuffer), _indexCount(indexCount), _indexBuffer(indexBuffer), _texture(texture)
{
}

void Mesh::destroy(VkDevice device, VmaAllocator allocator)
{
    _vertexBuffer.destroy(device, allocator);
    _indexBuffer.destroy(device, allocator);
}

uint32_t Mesh::getVertexCount() const
{
    return _vertexCount;
}

VkBuffer Mesh::getVertexBuffer() const
{
    return _vertexBuffer.getBuffer();
}

uint32_t Mesh::getIndexCount() const
{
    return _indexCount;
}

VkBuffer Mesh::getIndexBuffer() const
{
    return _indexBuffer.getBuffer();
}

std::shared_ptr<Texture> Mesh::getTexture() const
{
    return _texture;
}
