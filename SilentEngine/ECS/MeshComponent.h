#pragma once

#include "Component.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED

#include "glm\glm.hpp"
#include "../ECS/Entity.h"
#include "../Mesh.h"
#include <memory>
#include <string>

class Material;

class MeshComponent : public Component {
public:
    void setMesh(std::shared_ptr<Mesh> mesh);

    void setMaterial(std::shared_ptr<Material> material);

    std::shared_ptr<Mesh> getMesh() const;

    std::shared_ptr<Material> getMaterial() const;

private:
    std::shared_ptr<Mesh> _mesh;
    std::shared_ptr<Material> _material;
};
