#include "DrawData.h"

#include <utility>

DrawData::DrawData(const std::shared_ptr<Camera>& camera) {
    view = camera->getViewMatrix();
    projection = camera->getProjectionMatrix();
    position = camera->getPosition();
}

void DrawData::addDrawCall(std::shared_ptr<Mesh> mesh, const std::shared_ptr<Pipeline>& pipeline, const glm::mat4 &model) {
    DrawCall drawCall;

    drawCall._mesh = std::move(mesh);

    drawCall.pipelineLayout = pipeline->getPipelineLayout();
    drawCall.pipeline = pipeline->getPipeline();

    drawCall.model = model;

    _drawCalls.emplace_back(drawCall);
}

std::vector<DrawCall> const &DrawData::getDrawCalls() const {
    return _drawCalls;
}
