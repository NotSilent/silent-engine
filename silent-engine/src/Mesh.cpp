#include "Mesh.h"

Mesh::Mesh(uint32_t vertexCount, const Buffer<Vertex>& vertexBuffer, uint32_t indexCount, const Buffer<uint32_t>& indexBuffer) :
    _vertexCount(vertexCount), _vertexBuffer(vertexBuffer), _indexCount(indexCount), _indexBuffer(indexBuffer)
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