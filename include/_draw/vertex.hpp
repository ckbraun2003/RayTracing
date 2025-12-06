#pragma once

#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 position; // Position
  float padding1;
  glm::vec3 normalVector; // Color
  float padding2;
};

struct Material {
  glm::vec4 color;
  float lightEmission;
  float padding1;
  float padding2;
  float padding3;

  Material(glm::vec4 color, float lightEmission)
    : color(color), lightEmission(lightEmission)
  {}
};