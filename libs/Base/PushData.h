#pragma once

#include "glm\glm.hpp"

struct PushData {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    PushData() = delete;

    PushData(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    PushData(const PushData &other) = delete;

    PushData &operator=(const PushData &other) = delete;

    PushData(PushData &&other) = default;

    PushData &operator=(PushData &&other) = default;
};