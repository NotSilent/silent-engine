#include "MeshComponent.h"
#include "Material.h"

void MeshComponent::setMesh(std::shared_ptr<Mesh> mesh)
{
    _mesh = mesh;
}

void MeshComponent::setMaterial(std::shared_ptr<Material> material)
{
    _material = material;
}

std::shared_ptr<Mesh> MeshComponent::getMesh() const
{
    return _mesh;
}

std::shared_ptr<Material> MeshComponent::getMaterial() const
{
    return _material;
}
