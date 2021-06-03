#include "Mesh.h"

Mesh::Mesh(const uint32_t vertexCount, const Buffer<Vertex>& vertexBuffer, const uint32_t indexCount, const Buffer<uint32_t> indexBuffer)
{
    _vertexCount = vertexCount;
    _vertexBuffer = vertexBuffer;
    _indexCount = indexCount;
    _indexBuffer = indexBuffer;
}

Mesh::~Mesh()
{
    _vertexBuffer.destroy();
    _indexBuffer.destroy();
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
