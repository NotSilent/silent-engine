#pragma once

#include "Component.h"
#include <vector>
#include <memory>

class Entity {
public:
	Entity();
	~Entity();

	virtual void update(float deltaTime);

	void addComponent(std::shared_ptr<Component> component);

	void destroy();

private:
	std::vector<std::shared_ptr<Component>> _components;
};
