#pragma once

#include "Camera.h"
#include "Mesh.h"
#include <Texture.h>
#include <vector>

struct DrawCall {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Texture> texture;
    glm::mat4 model;
};

class DrawData {
public:
    DrawData(std::shared_ptr<Camera> camera);
    ~DrawData();

    void addDrawCall(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, const glm::mat4& model);

    std::shared_ptr<Camera> getCamera() const;
    std::vector<DrawCall> getDrawCalls() const;

private:
    std::shared_ptr<Camera> _camera;
    std::vector<DrawCall> _drawCalls;
};
