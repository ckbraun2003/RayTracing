#include <glad/glad.h>
#ifndef GL_SHADER_STORAGE_BUFFER
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#endif

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cmath>

#include "shader.hpp"
#include "world.hpp"
#include "_utils.hpp"

#include "_draw/triangle.hpp"
#include "_draw/sphere.hpp"
#include "_draw/quad.hpp"
#include "_draw/cube.hpp"

int width = 1280;
int height = 720;
float fov = 90;

float bounceCount = 1;
int numRays = 1;

WindowConfig window_config(width, height, fov);
World world;

glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  width / 2.0;
float lastY =  height / 2.0;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
};

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f; // change this value to your liking
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  fov -= (float)yoffset;
  if (fov < 1.0f)
    fov = 1.0f;
  if (fov > 90)
    fov = 90.0f;
}

int main()
{
  Material lightSource(glm::vec4(1.0, 1.0, 1.0, 1.0), 10.0);
  Material sphereMaterial(glm::vec4(1.0, 0.0, 0.0, 1.0), 0.0);
  Material triangleMaterial(glm::vec4(0.0, 1.0, 0.0, 1.0), 0.0);

  // Quad quad = Quad(
  //   Vertex{glm::vec3(1.0, 0.0, 1.0)},
  //   Vertex{glm::vec3(1.0, 0.0, -1.0)},
  //   Vertex{glm::vec3(-1.0, 0.0, -1.0)},
  //   Vertex{glm::vec3(-1.0, 0.0, 1.0)},
  //   triangleMaterial);

  Cube cube = Cube(glm::vec3(0.0), glm::vec3(5, 5, 5), triangleMaterial);
  world.AddObject(cube.ToMesh());
  // world.AddObject(quad.ToMesh());

  world.AddObject(Sphere(glm::vec3(3.0, 5.0, 3.0), 0.5, 32, 32, sphereMaterial));
  world.AddObject(Sphere(glm::vec3(-3.0, 5.0, -3.0), 0.5, 32, 32, sphereMaterial));
  world.AddObject(Sphere(glm::vec3(3.0, 5.0, -3.0), 0.5, 32, 32, sphereMaterial));
  world.AddObject(Sphere(glm::vec3(-3.0, 5.0, 3.0), 0.5, 32, 32, sphereMaterial));

  world.AddObject(Sphere(glm::vec3(0.0, 10.0, 0.0), 3, 32, 32, lightSource));


  std::vector<GPUSphere> spheres;
  for (auto& sphere : world.spheres) {
    spheres.push_back(sphere.GetGPUSphere());
  }

  std::vector<GPUMesh> meshes;
  std::vector<GPUTriangle> triangles;
  for (auto& mesh : world.meshes) {
    for (auto& triangle : mesh.triangles) {
      triangles.push_back(triangle.GetGPUTriangle());
    }
    meshes.push_back(mesh.GetGPUMesh());
  }

  // Initializes the Window Configuration for GLFW (need to look into more)
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Should be called before any OpenGL functions, makes sure GLAD is working
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // Tells OpenGL size of rendering window
  glViewport(0, 0, window_config.w, window_config.h);
  glEnable(GL_DEPTH_TEST);

  // Calculate model, view, projection matrices
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 projection = glm::perspective(glm::radians(window_config.fov),
    (float)window_config.w / (float)window_config.h, 0.1f, 100.0f);


  // Tells OpenGL to change render size when window size has changed
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Create VBO or Vertex Buffer Object, it is how we store vertexes in GPU memory
  unsigned int VBO;
  glGenBuffers(1, &VBO); // Creates a buffer with an ID
  glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Binds any changes made to that buffer to target
  glBufferData(GL_ARRAY_BUFFER, world.sceneVertices.size() * sizeof(Vertex), world.sceneVertices.data(), GL_STATIC_DRAW);

  // Creates VAO or Vertex Array Object, This is like a state manager for vertex attribute changes
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Creates EBO for storing Indices?
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, world.sceneIndices.size() * sizeof(unsigned int), world.sceneIndices.data(), GL_STATIC_DRAW);

  unsigned int SSBO_Spheres; // Use a descriptive name
  glGenBuffers(1, &SSBO_Spheres);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Spheres);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
                 spheres.size() * sizeof(GPUSphere),
                 spheres.data(),
                 GL_STATIC_DRAW);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO_Spheres);


  unsigned int SSBO_Triangles; // Another descriptive name
  glGenBuffers(1, &SSBO_Triangles);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Triangles);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
                 triangles.size() * sizeof(GPUTriangle), // Use the size of your second data type
                 triangles.data(),
                 GL_STATIC_DRAW);

  // Bind SSBO_OtherData to binding point 1
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBO_Triangles);

  unsigned int SSBO_Meshes; // Another descriptive name
  glGenBuffers(1, &SSBO_Meshes);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Meshes);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
                 meshes.size() * sizeof(GPUMesh), // Use the size of your second data type
                 meshes.data(),
                 GL_STATIC_DRAW);

  // Bind SSBO_OtherData to binding point 1
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO_Meshes);


  // Unbind the generic GL_SHADER_STORAGE_BUFFER target when done
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  // Creates Vertex Attribute Pointers for the vector buffer
  // Position attribute (location = 0) aPosition
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

  // Unbind Vertex Array
  glBindVertexArray(0);

  // Create Shader object with vertex and fragment files, simply handles the compilation and deletion
  Shader shader = Shader("glsl/basic_vertex.vert", "glsl/RayTracing_sphere_fragment.frag");

  // Main render loop
  while(!glfwWindowShouldClose(window))
  {
    processInput(window);

    glfwGetFramebufferSize(window, &window_config.w, &window_config.h);

    // CRITICAL: Update the viewport in case the window size changed via drag/resize
    glViewport(0, 0, window_config.w, window_config.h); // Add this here

    // CLear Buffer
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.001f));
    //view = glm::rotate(view, 0.001f, glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.001f));
    //model = glm::rotate(model, 0.002f, glm::vec3(0.0f, 1.0f, 0.0f));
    // const float radius = 10.0f;
    // float camX = sin(glfwGetTime()) * radius;
    // float camZ = cos(glfwGetTime()) * radius;
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Use shader and send the NEW MVP matrix
    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec2("uResolution", glm::vec2(window_config.w, window_config.h));
    shader.setFloat("uTime", glfwGetTime());
    shader.setVec3("uCameraPos", cameraPos);
    shader.setInt("uNumSpheres", spheres.size());
    shader.setInt("uNumMeshes", meshes.size());
    shader.setFloat("uBounceCount", bounceCount);
    shader.setInt("uNumRays", numRays);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, world.sceneIndices.size(), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();

  }
  // Cleaning up Buffers
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteBuffers(1, &SSBO_Spheres);
  glDeleteBuffers(1, &SSBO_Triangles);
  glDeleteBuffers(1, &SSBO_Meshes);
  glDeleteProgram(shader.ID);


  // Cleans up memory on termination
  glfwTerminate();
  return 0;

}

