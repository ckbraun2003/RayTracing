#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "objects.hpp"
#include "vertex.hpp"

struct Plane : public Object {
public:
  Plane(glm::vec3 center, float size, glm::vec4 color)
    : Object(center, color)
    , size(size)
  {
    indices = {0, 1, 2}; // Direct indices for the 3 vertices

    // Calculate positions relative to the center
    float halfSize = size / 2.0f;
    vertices.push_back({center + glm::vec3( 0.0f,  halfSize, 0.0f), color}); // Top
    vertices.push_back({center + glm::vec3(-halfSize, -halfSize, 0.0f), color}); // Bottom-Left
    vertices.push_back({center + glm::vec3( halfSize, -halfSize, 0.0f), color}); // Bottom-Right
  }
private:
  float size;
};