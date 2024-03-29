#include "Entity.h"
#include "Component.h"
#include "glm/ext/matrix_transform.hpp"

void Entity::update(float deltaTime)
{
    //_model = glm::rotate(_model, std::sin(deltaTime), glm::vec3 { 0.0f, 1.0f, 0.0f });
    for (auto& component : _components) {
        component->update(deltaTime);
    }
}

glm::mat4 Entity::getModel()
{
    return _model;
}

void Entity::translate(const glm::vec3& translation)
{
    _model = glm::translate(_model, translation);
}

void Entity::rotate(float angle, const glm::vec3& axis) {
    _model = glm::rotate(_model, angle, axis);
}

void Entity::setScale(float scale) {
    setScale(glm::vec3(scale));
}

void Entity::setScale(const glm::vec3& scale)
{
    _model = glm::scale(_model, scale);
}

void Entity::addComponent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> component)
{
    component->setEntity(entity);
    entity->_components.push_back(component);
}