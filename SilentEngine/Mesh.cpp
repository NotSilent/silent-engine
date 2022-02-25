#include "Mesh.h"

Mesh::Mesh(uint32_t indexCount, std::shared_ptr<Buffer> indexBuffer, const std::vector<VertexAttribute>& attributes)
    : _indexCount(indexCount)
    , _indexBuffer(indexBuffer)
    , _attributes(attributes)
{
}

void Mesh::destroy(VkDevice device, VmaAllocator allocator)
{
    //_vertexBuffer.destroy(device, allocator);
    //_indexBuffer.destroy(device, allocator);
}

uint32_t Mesh::getIndexCount() const
{
    return _indexCount;
}

VkBuffer Mesh::getIndexBuffer() const
{
    return _indexBuffer->getBuffer();
}

std::vector<VertexAttribute> const& Mesh::getAttributes() const
{
    return _attributes;
}
