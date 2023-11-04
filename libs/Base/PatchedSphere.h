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
    explicit PatchedSphere(uint32_t subdivisions);

    [[nodiscard]] std::vector<uint32_t > getIndices() const;
    [[nodiscard]] std::vector<glm::vec3> getPositions() const;;
    [[nodiscard]] std::vector<glm::vec3> getNormals() const;

private:
    std::vector<uint32_t> indices;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;

    [[nodiscard]] std::vector<Face> generateSubdividedFaces(const std::vector<Face>& faces) const;

    [[nodiscard]] uint32_t getIndexOfVertex(const glm::vec3& vertex);
};