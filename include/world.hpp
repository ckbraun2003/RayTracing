#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "_draw/vertex.hpp"
#include "_draw/sphere.hpp"
#include "_draw/triangle.hpp"
#include "_draw/mesh.hpp"

struct World {
  std::vector<Vertex> sceneVertices;
  std::vector<unsigned int> sceneIndices;

  std::vector<Sphere> spheres;
  std::vector<Mesh> meshes;

  void AddObject(Sphere sphere)
  {
    spheres.push_back(sphere);
    // A. Append all raw vertices to the master list
    sceneVertices.insert(sceneVertices.end(), sphere.vertices.begin(), sphere.vertices.end());

    // B. Append all indices to the master list, ADJUSTED by the current offset
    for (unsigned int index : sphere.indices) {
      // Add the base index (offset) to the object's local index
      sceneIndices.push_back(index + vertexOffset);
    }

    // C. Update the offset for the next object based on how many vertices were just added
    vertexOffset += sphere.vertices.size();
  }

  void AddObject(Triangle triangle)
  {
    // A. Append all raw vertices to the master list
    sceneVertices.push_back(triangle.vertexA);
    sceneVertices.push_back(triangle.vertexB);
    sceneVertices.push_back(triangle.vertexC);

    // B. Append all indices to the master list, ADJUSTED by the current offset
    for (unsigned int index : triangle.indices) {
      // Add the base index (offset) to the object's local index
      sceneIndices.push_back(index + vertexOffset);
    }

    // C. Update the offset for the next object based on how many vertices were just added
    vertexOffset += 3;
    meshOffset += 1;
  }

  void AddObject(Mesh mesh) {
    mesh.startIndex = meshOffset;
    meshes.push_back(mesh);

    for (auto& triangle : mesh.triangles) {
      AddObject(triangle);
    }
  }

private:
  unsigned int vertexOffset = 0;
  unsigned int meshOffset = 0;

};