#pragma once

#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

struct WindowConfig {
  int w;
  int h;
  float fov;
};