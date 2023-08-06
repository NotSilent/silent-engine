#pragma once

#include <memory>
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
