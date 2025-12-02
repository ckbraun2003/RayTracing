#pragma once

#include <vector>
#include <memory>

#include "_draw/vertex.hpp"

struct Object {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

};

struct Objects {
  std::vector<Vertex> sceneVertices;
  std::vector<unsigned int> sceneIndices;

  std::vector<std::shared_ptr<Object>> sceneObjects;

  void AddObject(std::shared_ptr<Object> obj)
  {
    sceneObjects.push_back(obj);

    // A. Append all raw vertices to the master list
    sceneVertices.insert(sceneVertices.end(), obj->vertices.begin(), obj->vertices.end());

    // B. Append all indices to the master list, ADJUSTED by the current offset
    for (unsigned int index : obj->indices) {
      // Add the base index (offset) to the object's local index
      sceneIndices.push_back(index + vertexOffset);
    }

    // C. Update the offset for the next object based on how many vertices were just added
    vertexOffset += obj->vertices.size();
  }

private:
  unsigned int vertexOffset = 0;

};