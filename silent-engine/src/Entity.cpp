#include "Entity.h"

Entity::Entity()
{
}

Entity::~Entity()
{
}

void Entity::update(float deltaTime)
{
	for(auto& component : _components) { 
		component->update(deltaTime);
	}
}

void Entity::addComponent(std::shared_ptr<Component> component)
{
	_components.push_back(component);
}

void Entity::destroy()
{
}
