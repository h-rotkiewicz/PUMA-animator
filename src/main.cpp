
#define STDOUT
#define DEBUG
#include "main.h"
#include "OBJ_Loader.h"
#include "defines.h"
#include "logger.h"
#include "paths.h"
#include "shader.h"
#include <array>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using buffer = unsigned int;

template <typename T, typename... N>
auto make_array(N &&...args) -> std::array<T, sizeof...(args)> {
  return {std::forward<N>(args)...};
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, float &angle, glm::vec3 &RotationAxis);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLFWwindow *init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Puma", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    throw std::runtime_error("Failed to init GLAD ");
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  return window;
}

std::tuple<buffer, buffer, buffer>
bindBuffers(const std::vector<objl::Vertex> &vertices,
            const std::vector<unsigned int> &indices) {
  buffer VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(objl::Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)offsetof(objl::Vertex, Position));
  glEnableVertexAttribArray(0);


  glBindVertexArray(0); // Unbind VAO
  return {VAO, VBO, EBO};
}

buffer bindTexture() {
  unsigned int texture1;
  // texture 1
  // ---------
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(
      true); // tell stb_image.h to flip loaded texture's on the y-axis.
  unsigned char *data = stbi_load(Paths::resources_metalic.c_str(), &width,
                                  &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  return texture1;
}

int main() {
  GLFWwindow *window = init();
  objl::Loader Loader;
  LOG::logger logger("Log.txt");
  bool loadout = Loader.LoadFile(Paths::resources_puma_obj);

  if (!loadout) {
    std::cerr << "Failed to load OBJ file." << std::endl;
    return -1;
  }

  Shader Shader(Paths::shaders_vs.c_str(), Paths::shaders_fs.c_str());
  auto vertices = Loader.LoadedVertices;
  auto indices = Loader.LoadedIndices;
  auto [VAO, VBO, EBO] = bindBuffers(vertices, indices);
  buffer texture1 =
      bindTexture(); // Update this as needed based on your texture

#ifdef DEBUG
  logger.log("Loaded Vertices: " + std::to_string(vertices.size()),
             ColorMod::Code::FG_GREEN);
  logger.log("Loaded Indices: " + std::to_string(indices.size()),
             ColorMod::Code::FG_GREEN);
#endif

  Shader.use();
  Shader.setInt("texture", 0);

  float angle{};
  glm::vec3 RotationAxis = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 modelPosition = glm::vec3(0.0f, -0.5f, 0.0f);

  glEnable(GL_DEPTH_TEST);


    Shader.use();
  while (!glfwWindowShouldClose(window)) {
    processInput(window, angle, RotationAxis);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);


    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    projection =
        glm::perspective(glm::radians(45.0f),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    Shader.setMat4("projection", projection);
    Shader.setMat4("view", view);
    Shader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));

    glBindVertexArray(VAO);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition);
    angle = 2 * cos(glfwGetTime());
    model = glm::rotate(model, glm::radians(angle), RotationAxis);

    Shader.setMat4("model", model);
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0); // traiangle strip Works better ???? why ??

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwTerminate();
  return 0;
}
void processInput(GLFWwindow *window, float &angle, glm::vec3 &RotationAxis) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}
