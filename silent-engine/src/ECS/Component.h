#pragma once
#include <memory>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "glm\glm.hpp"

class Entity;

class Component {
public:
	virtual void update(float deltaTime);
	
    void setEntity(std::shared_ptr<Entity> entity);

	glm::mat4 getModel();

protected:	
    std::weak_ptr<Entity> _entity;

private:
};
