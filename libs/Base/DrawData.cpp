#include "DrawData.h"
#include "Material.h"

DrawData::DrawData(std::shared_ptr<Camera> camera)
{
    _camera = camera;
}

DrawData::~DrawData()
{
}

void DrawData::addDrawCall(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4& model)
{
    _drawCalls.push_back({
        .mesh = mesh,
        .material = material,
        .model = model,
    });
}

std::shared_ptr<Camera> DrawData::getCamera() const
{
    return _camera;
}

std::vector<DrawCall> const& DrawData::getDrawCalls() const
{
    return _drawCalls;
}
