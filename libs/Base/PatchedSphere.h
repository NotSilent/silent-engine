#pragma once

// Inspired by
// https://iquilezles.org/articles/patchedsphere/

#include <vector>
#include <array>
#include <glm/glm.hpp>

struct Face {
    glm::vec3 topLeft;
    glm::vec3 topRight;
    glm::vec3 bottomLeft;
    glm::vec3 bottomRight;
};

class PatchedSphere {
public:
    PatchedSphere(uint32_t subdivisions);

    [[nodiscard]] std::vector<glm::vec3> getVertices() const;
    [[nodiscard]] std::vector<int32_t > getIndices() const;

private:
    std::vector<glm::vec3> vertices;
    std::vector<int32_t> indices;

    [[nodiscard]] std::vector<Face> generateSubdividedFaces(const std::vector<Face>& faces) const;

    [[nodiscard]] uint32_t getIndexOfVertex(const glm::vec3& vertex);
};