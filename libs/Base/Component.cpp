#include "Component.h"
#include "Entity.h"

void Component::update(float deltaTime)
{
}

void Component::setEntity(std::shared_ptr<Entity> entity)
{
    _entity = entity;
}

glm::mat4 Component::getModel()
{
    return _entity.lock()->getModel();
}
