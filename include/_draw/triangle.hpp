#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "vertex.hpp"

struct GPUTriangle {
  GPUTriangle(Vertex vertexA, Vertex vertexB, Vertex vertexC, glm::vec3 normalVector, Material material)
    : vertexA(vertexA), vertexB(vertexB), vertexC(vertexC), normalVector(normalVector), material(material)
  { }

  Vertex vertexA;
  Vertex vertexB;
  Vertex vertexC;

  glm::vec3 normalVector;
  float padding4;

  Material material;
};

struct Triangle {
  Triangle(Vertex vertexA, Vertex vertexB, Vertex vertexC, Material material)
  : vertexA(vertexA), vertexB(vertexB), vertexC(vertexC),  material(material)
  {
    glm::vec3 edge1 = vertexB.position - vertexA.position;
    glm::vec3 edge2 = vertexC.position - vertexA.position;

    // 2. Calculate the cross product
    glm::vec3 normal = cross(edge1, edge2);

    // 3. Normalize the result to get a unit vector
    normalVector = glm::normalize(normal);

    indices = {0, 1, 2}; // Direct indices for the 3 vertices
  }

  Triangle(Material material)
  : vertexA(Vertex(glm::vec3(0.0))), vertexB(Vertex(glm::vec3(1.0, 0.0, 0.0))), vertexC(Vertex(glm::vec3(0.0, 0.0, 1.0))),  material(material)
  {
    glm::vec3 edge1 = vertexB.position - vertexA.position;
    glm::vec3 edge2 = vertexC.position - vertexA.position;

    // 2. Calculate the cross product
    glm::vec3 normal = cross(edge1, edge2);

    // 3. Normalize the result to get a unit vector
    normalVector = glm::normalize(normal);

    indices = {0, 1, 2}; // Direct indices for the 3 vertices
  }

  GPUTriangle GetGPUTriangle() {
    return GPUTriangle{vertexA, vertexB, vertexC, normalVector, material};
  }

  Vertex vertexA;
  Vertex vertexB;
  Vertex vertexC;
  std::vector<unsigned int> indices;
private:
  glm::vec3 normalVector;

  Material material;
};