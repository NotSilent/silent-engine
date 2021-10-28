#pragma once

#include "Camera.h"
#include "Mesh.h"
#include <vector>
#include <Texture.h>

using DrawCall = std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Texture>>;

class DrawData {
public:
	DrawData(const Camera& camera);
	~DrawData();

	void addDrawCall(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);

	Camera getCamera() const;
	std::vector<DrawCall> getDrawCalls() const;

private:
	Camera _camera;
	std::vector<DrawCall> _drawCalls;
};
