#include "DrawData.h"
#include "Material.h"

DrawData::DrawData(std::shared_ptr<Camera> camera) {
    view = camera->getViewMatrix();
    projection = camera->getProjectionMatrix();
    position = camera->getPosition();
}

DrawData::~DrawData() {
}

void DrawData::addDrawCall(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4 &model) {
    DrawCall drawCall;

    drawCall._mesh = mesh;

    drawCall.pipelineLayout = material->getPipelineLayout();
    drawCall.pipeline = material->getPipeline();

    drawCall.model = model;

    _drawCalls.emplace_back(drawCall);
}

std::vector<DrawCall> const &DrawData::getDrawCalls() const {
    return _drawCalls;
}
