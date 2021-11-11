#pragma once
#include "glm\glm.hpp"

struct PushData {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 viewPosition;
};