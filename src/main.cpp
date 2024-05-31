
#include <unordered_map>

#include "camera.h"
#define DEBUG
#include <cmath>

#include "Rendering.h"
#include "animEng.h"
#include "common.h"
#include "defines.h"
#include "paths.h"

void make_robot(std::unordered_map<RobotParts, Part> &container, ShaderManager &AnimationEng) {
  auto make_part = [&container, &AnimationEng](RobotParts part, const char *path) {
    container.try_emplace(part, Part(path));
    AnimationEng.addShader(part, Shader(Paths::shaders_vs, Paths::shaders_fs));
  };

  // WARNING: make sure the initialization of parts is in correct order, otherwise the rotations with be bugged
  make_part(RobotParts::hand, Paths::resources_hand);
  make_part(RobotParts::forearm, Paths::resources_Forearm);
  make_part(RobotParts::joint, Paths::resources_joint);
  make_part(RobotParts::middle_arm, Paths::resources_middle_arm_obj);
  make_part(RobotParts::upper_base, Paths::resources_upper_base_obj);
  make_part(RobotParts::base, Paths::resources_base_obj);
}

void processInput(GLFWwindow *window, Camera &camera, std::unordered_map<RobotParts, Part> &Parts, ShaderManager &AnimationEng) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.Orbit(1.f, glm::vec3(1.0f, 0.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.Orbit(-1.f, glm::vec3(1.0f, 0.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.Orbit(1.f, glm::vec3(0.0f, 1.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.Orbit(-1.f, glm::vec3(0.0f, 1.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.change_vert_offset(0.01);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.change_vert_offset(-0.01);
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) camera.change_radius(0.01);
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) camera.change_radius(-0.01);
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) AnimationEng.RotatePart(RobotParts::upper_base, 1.f);
  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) AnimationEng.RotatePart(RobotParts::middle_arm, 1.f);
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) AnimationEng.RotatePart(RobotParts::joint, 1.f);
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) AnimationEng.RotatePart(RobotParts::forearm, 1.f);
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) AnimationEng.RotatePart(RobotParts::hand, 1.f);
}

int main() {
  {
    int           Radius = 3;
    GLFWwindow   *window = init();
    Camera        camera(Radius, 0, {0.0f, 0.0f, 0.0f});
    Shader        shader(Paths::shaders_vs, Paths::shaders_fs);
    ShaderManager AnimationEng;

    std::unordered_map<RobotParts, Part> Parts;
    make_robot(Parts, AnimationEng);

    while (!glfwWindowShouldClose(window)) {
      preRender();
      processInput(window, camera, Parts, AnimationEng);
      AnimationEng.updateShader(camera,window);
      AnimationEng.render(Parts, window);
      CheckForErrors("Something went wrong ");
    }
  }
  glfwTerminate();
  return 0;
}
