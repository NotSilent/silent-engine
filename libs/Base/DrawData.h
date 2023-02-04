#pragma once

#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include <vector>
#include "Material.h"

struct DrawCall {
    std::shared_ptr<Mesh> _mesh;

    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    glm::mat4 model;
};

class DrawData {
public:
    DrawData(std::shared_ptr<Camera> camera);

    ~DrawData();

    void addDrawCall(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4 &model);

    std::vector<DrawCall> const &getDrawCalls() const;

    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 position;

private:
    std::vector<DrawCall> _drawCalls;
};
