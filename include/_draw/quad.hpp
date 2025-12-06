#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "_draw/triangle.hpp"
#include "_draw/mesh.hpp"

struct Quad {
  Quad(Vertex vertexA, Vertex vertexB, Vertex vertexC, Vertex vertexD, Material material)
  : triangleA{vertexA, vertexB, vertexC, material}
  , triangleB{vertexC, vertexD, vertexA, material}
  , material(material)
  { }

  Mesh ToMesh() {
    return Mesh(std::vector{triangleA, triangleB}, boundsMin, boundsMax);
  }

  Triangle triangleA;
  Triangle triangleB;
private:
  Material material;
  glm::vec3 boundsMin;
  glm::vec3 boundsMax;
};