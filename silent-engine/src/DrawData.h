#pragma once

#include "Camera.h"
#include "Mesh.h"
#include <vector>
#include <Texture.h>

struct DrawCall {
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Texture> texture;
	glm::mat4 model;
};

class DrawData {
public:
	DrawData(const Camera& camera);
	~DrawData();

	void addDrawCall(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, const glm::mat4& model);

	Camera getCamera() const;
	std::vector<DrawCall> getDrawCalls() const;

private:
	Camera _camera;
	std::vector<DrawCall> _drawCalls;
};
