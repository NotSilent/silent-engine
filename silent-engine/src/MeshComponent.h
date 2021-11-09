#pragma once

#include "Component.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "glm\glm.hpp"
#include <Mesh.h>
#include <memory>
#include <string>
#include "Texture.h"
#include <Entity.h>

class MeshComponent : public Component {
public:
    void setMesh(std::shared_ptr<Mesh> mesh);
    void setTexture(std::shared_ptr<Texture> texture);

    std::shared_ptr<Mesh> getMesh() const;
    std::shared_ptr<Texture> getTexture() const;

private:
    std::shared_ptr<Mesh> _mesh;
    std::shared_ptr<Texture> _texture;
};
