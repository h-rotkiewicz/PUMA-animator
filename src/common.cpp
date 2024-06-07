#include "common.h"

#include <iostream>
#include <string>

#include "OBJ_Loader.h"
#include "settings.h"
#include "stb_image.h"
void framebufferSizeCallback([[maybe_unused]] GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); }

void CheckForErrors(std::string_view message) {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) std::cerr << message << " || OpenGL Error: " << error << std::endl;
}
void preRender() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow *init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(Settings::Window_Width, Settings::Window_Height, "Puma", NULL, NULL);
  if (window == nullptr) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::runtime_error("Failed to init GLAD ");
  glEnable(GL_DEPTH_TEST);
  return window;
}


GLBuffers bindBuffers(std::vector<objl::Vertex> const &vertices, std::vector<unsigned int> const &indices) {
  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(objl::Vertex), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
  CheckForErrors("bindBuffers");

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void *)offsetof(objl::Vertex, Position));
  glEnableVertexAttribArray(0);
  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void *)offsetof(objl::Vertex, Normal));
  glEnableVertexAttribArray(1);
  // Tex coordinate attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void *)offsetof(objl::Vertex, TextureCoordinate));
  glEnableVertexAttribArray(2);
  CheckForErrors("Attributes");

  glBindVertexArray(0);  // Unbind VAO
  return {VAO, VBO, EBO, indices.size()};
}

GLBuffers load_vxo(std::string_view path) {
  objl::Loader Loader;
  if (!Loader.LoadFile(path.data())) {
    std::cerr << "Failed to load OBJ file." << path.data() << std::endl;
    throw std::runtime_error("Failed to load OBJ file.");
  }
  return bindBuffers(Loader.LoadedVertices, Loader.LoadedIndices);
}

