#pragma once

#include "Component.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED

#include "glm\glm.hpp"
#include "Entity.h"
#include "../Base/Mesh.h"
#include <memory>
#include <string>

class Material;

class MeshComponent : public Component {
public:
    void setMesh(std::shared_ptr<Mesh> mesh);

    void setMaterial(std::shared_ptr<Material> material);

    [[nodiscard]] std::shared_ptr<Mesh> getMesh() const;

    [[nodiscard]] std::shared_ptr<Material> getMaterial() const;

private:
    std::shared_ptr<Mesh> _mesh;
    std::shared_ptr<Material> _material;
};
