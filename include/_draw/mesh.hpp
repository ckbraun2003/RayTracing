#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "_draw/triangle.hpp"

struct GPUMesh {
  GPUMesh(unsigned int startIndex, unsigned int size, glm::vec3 boundsMin, glm::vec3 boundsMax)
    : startIndex(startIndex), size(size), boundsMin(boundsMin), boundsMax(boundsMax)
  { }

  unsigned int startIndex;
  unsigned int size;
  glm::vec3 boundsMin;
  glm::vec3 boundsMax;
};

struct Mesh {
  Mesh(std::vector<Triangle> triangles, glm::vec3 boundsMin, glm::vec3 boundsMax)
    : triangles(triangles), boundsMin(boundsMin), boundsMax(boundsMax)
  { }

  GPUMesh GetGPUMesh() {
    return GPUMesh(startIndex, triangles.size(), boundsMin, boundsMax);
  }

  std::vector<Triangle> triangles;
  unsigned int startIndex;
private:

  glm::vec3 boundsMin;
  glm::vec3 boundsMax;
};