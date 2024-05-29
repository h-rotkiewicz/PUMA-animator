
#include <unordered_map>

#include "ModelParams.h"
#define DEBUG
#include <cmath>

#include "Rendering.h"
#include "animEng.h"
#include "common.h"
#include "defines.h"
#include "paths.h"

void make_robot(std::unordered_map<RobotParts, Part> &container, ShaderManager &AnimationEng) {
  Part part(Paths::resources_base_obj.string());

  AnimationEng.addShader(RobotParts::middle_arm, Shader(Paths::shaders_vs.c_str(), Paths::shaders_fs.c_str()));
  AnimationEng.addShader(RobotParts::upper_base, Shader(Paths::shaders_vs.c_str(), Paths::shaders_fs.c_str()));
  AnimationEng.addShader(RobotParts::base, Shader(Paths::shaders_vs.c_str(), Paths::shaders_fs.c_str()));

  // WARNING: make sure the initialization of parts is in correct order, otherwise the rotations with be bugged
  container.emplace(RobotParts::middle_arm, Part(Paths::resources_middle_arm_obj.string()));
  container.emplace(RobotParts::upper_base, Part(Paths::resources_upper_base_obj.string()));
  container.emplace(RobotParts::base, Part(Paths::resources_base_obj.string()));
}

void processInput(GLFWwindow *window, Camera &camera, std::unordered_map<RobotParts, Part> &Parts, ShaderManager &AnimationEng) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.Orbit(1.f, glm::vec3(1.0f, 0.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.Orbit(-1.f, glm::vec3(1.0f, 0.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.Orbit(1.f, glm::vec3(0.0f, 1.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.Orbit(-1.f, glm::vec3(0.0f, 1.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera.change_vert_offset(0.01);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera.change_vert_offset(-0.01);
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    camera.change_radius(0.01);
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    camera.change_radius(-0.01);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
  {
    // AnimationEng.RotatePart(RobotParts::upper_base, 1.f, glm::vec3(0.0f, 1.0f, 0.0f));
    AnimationEng.RotatePart(RobotParts::middle_arm, 1.0f);
  }
}

int main() {
  int           Radius = 3;
  GLFWwindow   *window = init();
  Camera        camera(Radius, 0, {0.0f, 0.0f, 0.0f});
  Shader        shader(Paths::shaders_vs.c_str(), Paths::shaders_fs.c_str());
  ShaderManager AnimationEng;

  std::unordered_map<RobotParts, Part> Parts;
  make_robot(Parts, AnimationEng);

  while (!glfwWindowShouldClose(window)) {
    preRender();
    processInput(window, camera, Parts, AnimationEng);
    AnimationEng.updateShader(camera);
    AnimationEng.render(Parts, window);
    CheckForErrors("Something went wrong ");
  }

  glfwTerminate();
  return 0;
}
