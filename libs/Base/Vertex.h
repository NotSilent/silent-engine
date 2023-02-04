#pragma once

#include "glm/vec3.hpp"

struct Vertex {
    glm::vec3 position;

    Vertex(float x, float y, float z)
            : position(x, y, z) {

    }
};
