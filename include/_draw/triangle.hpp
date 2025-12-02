#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "objects.hpp"
#include "vertex.hpp"

class Triangle : public Object {
public:
  Triangle(glm::vec3 center, float size, glm::vec3 color)
  {
    indices = {0, 1, 2}; // Direct indices for the 3 vertices

    // Calculate positions relative to the center
    float halfSize = size / 2.0f;
    vertices.push_back({center + glm::vec3( 0.0f,  halfSize, 0.0f), color}); // Top
    vertices.push_back({center + glm::vec3(-halfSize, -halfSize, 0.0f), color}); // Bottom-Left
    vertices.push_back({center + glm::vec3( halfSize, -halfSize, 0.0f), color}); // Bottom-Right
  }
};