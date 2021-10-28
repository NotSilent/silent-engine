#include "MeshComponent.h"

glm::vec3 MeshComponent::getTranslation() const
{
    //TODO: Get parent Entity translation
    return glm::vec3(0.0f);
}

void MeshComponent::setMesh(std::shared_ptr<Mesh> mesh)
{
    _mesh = mesh;
}

void MeshComponent::setTexture(std::shared_ptr<Texture> texture)
{
    _texture = texture;
}

std::shared_ptr<Mesh> MeshComponent::getMesh() const
{
    return _mesh;
}

std::shared_ptr<Texture> MeshComponent::getTexture() const
{
    return _texture;
}
