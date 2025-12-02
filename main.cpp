#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

#include "shader.hpp"

int window_width = 800;
int window_height = 800;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

int main()
{

  // Initializes the Window Configuration for GLFW (need to look into more)
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Creates the GLFW Window with a Width, Height, Title, etc
  GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Shaders", NULL, NULL);

  // Error handling
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  // Makes the window the current context for the thread
  glfwMakeContextCurrent(window);

  // Should be called before any OpenGL functions, makes sure GLAD is working
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // Tells OpenGL size of rendering window
  glViewport(0, 0, window_width, window_height);

  // Tells OpenGL to change render size when window size has changed
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // ---RENDERING A RECTANGLE---
  float vertices[] = {
    1.0f,  1.0f, 0.0f,  // top right
    1.0f,  -1.0f, 0.0f,  // bottom right
    -1.0f,  -1.0f, 0.0f,  // bottom left
    -1.0f,  1.0f, 0.0f   // top left
  };
  unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
  };

  // Create VBO or Vertex Buffer Object, it is how we store vertexes in GPU memory
  unsigned int VBO;
  glGenBuffers(1, &VBO); // Creates a buffer with an ID
  glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Binds any changes made to that buffer to target
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies defined data into the bound buffer

  // Creates VAO or Vertex Array Object, This is like a state manager for vertex attribute changes
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Creates Vertex Attribute Pointers for the vector buffer
  // Position attribute (location = 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  // Create Shader object with vertex and fragment files, simply handles the compilation and deletion
  Shader shader = Shader("glsl/basic_vertex.vert", "glsl/neon_circles_fragment.frag");

  // Main render loop
  while(!glfwWindowShouldClose(window))
  {
    processInput(window);

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    // CLear Buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Uses shader -- Sets Vertex attribute
    shader.use();
    shader.setVec2("uResolution", fb_width, fb_height);
    shader.setFloat("uTime", glfwGetTime());

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();

  }
  // Cleaning up Buffers
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shader.ID);

  // Cleans up memory on termination
  glfwTerminate();
  return 0;

}