#define Debug
#include <cmath>
#include <unordered_map>

#include "animEng.h"
#include "camera.h"
#include "common.h"
#include "defines.h"
#include "paths.h"

void make_robot(std::unordered_map<RobotParts, Part> &container, PartManager &AnimationEng) {
  auto make_part = [&container, &AnimationEng](RobotParts part, const char *path) {
    container.try_emplace(part, path);
    AnimationEng.addShader(part, Shader(Paths::shaders_vs, Paths::shaders_fs));
  };

  make_part(RobotParts::forearm, Paths::resources_Forearm);
  make_part(RobotParts::joint, Paths::resources_joint);
  make_part(RobotParts::middle_arm, Paths::resources_middle_arm_obj);
  make_part(RobotParts::hand, Paths::resources_hand);
  make_part(RobotParts::upper_base, Paths::resources_upper_base_obj);
  make_part(RobotParts::base, Paths::resources_base_obj);
}

void processInput(GLFWwindow *window, Camera &camera, PartManager &AnimationEng) {
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


template<typename T>
concept Container = requires(T t) {
  std::begin(t);
  std::end(t);
};

template<Container T>
void render_helper(T const & cont){
  for(auto const &elem : cont){
    elem.render();
  }
}

void render_helper(auto const & elem){
    elem.render();
}

void render(const PartManager &PartsManager, GLFWwindow *window, const std::unordered_map<RobotParts, Part> &RobotParts, auto const &... part) {
  PartsManager.render(RobotParts);
  (render_helper(part),...);
  CheckForErrors("render");
  glfwSwapBuffers(window);
  glfwPollEvents();
}

int main() {
  {
    int         Radius = 3;
    GLFWwindow *window = init();
    Camera      camera(Radius, 0, {0.0f, 0.0f, 0.0f});
    Shader      shader(Paths::shaders_vs, Paths::shaders_fs);
    Point       point({1, 1, 1}, {1,0,0});
    try {
      PartManager                          partManger;
      std::unordered_map<RobotParts, Part> Parts;
      make_robot(Parts, partManger);
      while (!glfwWindowShouldClose(window)) {
        preRender();
        processInput(window, camera, partManger);
        point.updateShaders(camera);
        partManger.updateShaders(camera);
        render(partManger, window, Parts, point);
        CheckForErrors("Something went wrong ");
      }
    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
    }
  }
  glfwTerminate();
  return 0;
}
