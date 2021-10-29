#pragma once

#include "Component.h"
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
