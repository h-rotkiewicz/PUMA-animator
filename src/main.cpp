
#define STDOUT
#define DEBUG
#include "main.h"
#include "Rendering.h"
#include "logger.h"
#include <cmath>


int main() {
  int Radius = 3;
  GLFWwindow *window = init();
  objl::Loader Loader;
  ModelParams modelParams;
  Camera camera(Radius, 0,modelParams.modelPosition);
  Shader Shader(Paths::shaders_vs.c_str(), Paths::shaders_fs.c_str());
  if (!Loader.LoadFile(Paths::resources_puma_obj)) {
    std::cerr << "Failed to load OBJ file." << std::endl;
    return -1;
  }

  auto vertices = Loader.LoadedVertices;
  auto indices = Loader.LoadedIndices;
  auto [VAO, VBO, EBO] = bindBuffers(vertices, indices);
  GLuint texture1 = bindTexture();

#ifdef DEBUG
  LOG::Logger.log("Loaded Vertices: " + std::to_string(vertices.size()),
             ColorMod::Code::FG_GREEN);
  LOG::Logger.log("Loaded Indices: " + std::to_string(indices.size()),
             ColorMod::Code::FG_GREEN);

  LOG::Logger.log("Model Position: " + std::to_string(modelParams.modelPosition.x) +
                 " " + std::to_string(modelParams.modelPosition.y) + " " +
                 std::to_string(modelParams.modelPosition.z),
             ColorMod::Code::FG_BLUE);
#endif

  glEnable(GL_DEPTH_TEST);
  Shader.use();
  Shader.setInt("texture", 0);

  while (!glfwWindowShouldClose(window)) {
    processInput(window,  camera);
    preRender(window, texture1, modelParams);
    updateShader(Shader, modelParams, camera);
    render(VAO, indices, window);
    modelParams.angle = 2 * cos(glfwGetTime());
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwTerminate();
  return 0;
}
