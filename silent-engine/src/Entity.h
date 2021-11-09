#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "glm/glm.hpp"
#include <memory>
#include <vector>

class Component;

class Entity {
public:
    virtual void update(float deltaTime);

    glm::mat4 getModel();

    void translate(const glm::vec3 translation);
    void setScale(const glm::vec3 scale);

    static void addComponent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> component);

private:
    std::vector<std::shared_ptr<Component>> _components;

    glm::mat4 _model { 1.0f };
};
