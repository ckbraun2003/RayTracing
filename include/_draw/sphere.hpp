#pragma once

#include <vector>
#include <cmath>
#include <glm/glm.hpp>

#include "objects.hpp"
#include "vertex.hpp"

struct Sphere : public Object {
public:
  Sphere(glm::vec3 center, float radius, int sectors, int stacks, glm::vec4 color)
    : Object(center, color)
    , radius(radius)
  {
    const float PI = acos(-1.0f);
    const float sectorStep = 2.0f * PI / sectors;
    const float stackStep = PI / stacks;

    for (int i = 0; i <= stacks; ++i) {
      float stackAngle = PI / 2.0f - i * stackStep; // angle from y-axis
      float xy = radius * cos(stackAngle);          // radius at this stack level
      float z = radius * sin(stackAngle);           // height (z is usually y in OpenGL)

      for (int j = 0; j <= sectors; ++j) {
        float sectorAngle = j * sectorStep;       // angle around the z-axis

        // Calculate vertex position using spherical coordinates
        float x = xy * cos(sectorAngle);
        float y = xy * sin(sectorAngle);

        // Add the vertex to the list
        vertices.push_back({glm::vec3(x, y, z) + center, color});
      }
    }

    // Generate the indices for connecting the vertices into triangles (quads)
    for (int i = 0; i < stacks; ++i) {
      for (int j = 0; j < sectors; ++j) {
        unsigned int k1 = i * (sectors + 1) + j;     // Current point
        unsigned int k2 = k1 + (sectors + 1);        // Point below
        unsigned int k3 = k1 + 1;                    // Point to the right
        unsigned int k4 = k2 + 1;                    // Point below and right

        // Two triangles form a quad (except at the poles)
        // Triangle 1
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k3);

        // Triangle 2
        indices.push_back(k3);
        indices.push_back(k2);
        indices.push_back(k4);
      }
    }
  }
private:
  float radius;
};
