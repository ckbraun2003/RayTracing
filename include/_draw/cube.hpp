#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "_draw/triangle.hpp"
#include "_draw/quad.hpp"
#include "_draw/mesh.hpp"

struct Cube {
  Cube(glm::vec3 center, glm::vec3 size, Material material)
  : center(center), size(size), material(material)
  {
    glm::vec3 h = size * 0.5f; // half extents

    // Define the 8 corners of the cube
    std::vector<Vertex> corners;
    corners.push_back(Vertex(center + glm::vec3(-h.x, -h.y,  h.z))), // 0: front-bottom-left
    corners.push_back(Vertex(center + glm::vec3( h.x, -h.y,  h.z))), // 1: front-bottom-right
    corners.push_back(Vertex(center + glm::vec3( h.x,  h.y,  h.z))), // 2: front-top-right
    corners.push_back(Vertex(center + glm::vec3(-h.x,  h.y,  h.z))), // 3: front-top-left
    corners.push_back(Vertex(center + glm::vec3(-h.x, -h.y, -h.z))), // 4: back-bottom-left
    corners.push_back(Vertex(center + glm::vec3( h.x, -h.y, -h.z))), // 5: back-bottom-right
    corners.push_back(Vertex(center + glm::vec3( h.x,  h.y, -h.z))), // 6: back-top-right
    corners.push_back(Vertex(center + glm::vec3(-h.x,  h.y, -h.z)));  // 7: back-top-left

    // Define each face as two triangles (12 triangles total)
    // Using counter-clockwise winding for outward-facing normals
    Quad face = Quad(corners[0], corners[1], corners[2], corners[3], material);
    // Front face (+Z)
    triangles.push_back(face.triangleA);
    triangles.push_back(face.triangleB);
    quads.push_back(face);

    // Back face (-Z)
    face = Quad(corners[5], corners[4], corners[7], corners[6], material);
    triangles.push_back(face.triangleA);
    triangles.push_back(face.triangleB);
    quads.push_back(face);

    // Right face (+X)
    face = Quad(corners[1], corners[5], corners[6], corners[2], material);
    triangles.push_back(face.triangleA);
    triangles.push_back(face.triangleB);
    quads.push_back(face);

    // Left face (-X)
    face = Quad(corners[4], corners[0], corners[3], corners[7], material);
    triangles.push_back(face.triangleA);
    triangles.push_back(face.triangleB);
    quads.push_back(face);

    // Top face (+Y)
    face = Quad(corners[3], corners[2], corners[6], corners[7], material);
    triangles.push_back(face.triangleA);
    triangles.push_back(face.triangleB);
    quads.push_back(face);

    // Bottom face (-Y)
    face = Quad(corners[4], corners[5], corners[1], corners[0], material);
    triangles.push_back(face.triangleA);
    triangles.push_back(face.triangleB);
    quads.push_back(face);
  }

  Mesh ToMesh() {
    return Mesh(triangles, boundsMin, boundsMax);
  }

  std::vector<Quad> quads;
  std::vector<Triangle> triangles;
private:
  glm::vec3 center;
  glm::vec3 size;
  Material material;

  glm::vec3 boundsMin;
  glm::vec3 boundsMax;
};