#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cmath>

#include "shader.hpp"
#include "objects.hpp"
#include "_utils.hpp"

#include "_draw/triangle.hpp"
#include "_draw/sphere.hpp"

int width = 800;
int height = 600;
float fov = 45;

int main()
{
  WindowConfig window_config(width, height, fov);
  Objects objects;

  // Add objects to Objects Structure
  objects.AddObject(std::make_shared<Sphere>(glm::vec3(0.0), 1.0, 32, 32, glm::vec3(0.0)));

  // Initializes the Window Configuration for GLFW (need to look into more)
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Creates the GLFW Window with a Width, Height, Title, etc
  GLFWwindow* window = glfwCreateWindow(window_config.w, window_config.h, "Shaders", NULL, NULL);

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
  glViewport(0, 0, window_config.w, window_config.h);
  glEnable(GL_DEPTH_TEST);

  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
  glm::mat4 model = glm::mat4(1.0f);

  // Tells OpenGL to change render size when window size has changed
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Create VBO or Vertex Buffer Object, it is how we store vertexes in GPU memory
  unsigned int VBO;
  glGenBuffers(1, &VBO); // Creates a buffer with an ID
  glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Binds any changes made to that buffer to target
  glBufferData(GL_ARRAY_BUFFER, objects.sceneVertices.size() * sizeof(Vertex), objects.sceneVertices.data(), GL_STATIC_DRAW);

  // Creates VAO or Vertex Array Object, This is like a state manager for vertex attribute changes
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, objects.sceneIndices.size() * sizeof(unsigned int), objects.sceneIndices.data(), GL_STATIC_DRAW);

  // Creates Vertex Attribute Pointers for the vector buffer
  // Position attribute (location = 0) aPosition
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

  // Position attribute (location = 1) aColor
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));

  // Unbind Vertex Array
  glBindVertexArray(0);

  // Create Shader object with vertex and fragment files, simply handles the compilation and deletion
  Shader shader = Shader("glsl/basic_vertex.vert", "glsl/ray_tracing_fragment.frag");

  // Main render loop
  while(!glfwWindowShouldClose(window))
  {
    processInput(window);

    glfwGetFramebufferSize(window, &window_config.w, &window_config.h);

    // CRITICAL: Update the viewport in case the window size changed via drag/resize
    glViewport(0, 0, window_config.w, window_config.h); // Add this here
    float aspect_ratio = (float)window_config.w / (float)window_config.h;
    glm::mat4 projection = glm::perspective(glm::radians(window_config.fov), aspect_ratio, 0.1f, 100.0f);

    // Recalculate the combined MVP matrix
    glm::mat4 mvp = projection * view * model;

    // CLear Buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use shader and send the NEW MVP matrix
    shader.use();
    shader.setMat4("uMVP", mvp);
    shader.setVec2("uResolution", window_config.w, window_config.h);
    shader.setFloat("uTime", glfwGetTime());

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, objects.sceneIndices.size(), GL_UNSIGNED_INT, 0);

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