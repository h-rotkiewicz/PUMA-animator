
#define STDOUT
#define DEBUG
#include "main.h"
#include "Rendering.h"
#include "defines.h"
#include "logger.h"
#include <cmath>


int main() {
  int Radius = 3;
  GLFWwindow *window = init();
  ModelParams modelParams;
  Camera camera(Radius, 0, modelParams.modelPosition);
  Shader Shader(Paths::shaders_vs.c_str(), Paths::shaders_fs.c_str());

  std::vector<Part> Parts;
  make_robot(Parts);

  Shader.use();
  // std::cout << "VAO: " << Parts[0].get_vao()<< std::endl;
  while (!glfwWindowShouldClose(window)) {
    processInput(window, camera);
    preRender();
    updateShader(Shader, modelParams, camera);
    render(Parts, window);
    // modelParams.angle = 2 * cos(glfwGetTime());
    CheckForErrors("Something went wrong ");
  }

  glfwTerminate();
  return 0;
}
