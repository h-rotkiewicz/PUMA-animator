#define Debug
#include <cmath>
#include <unordered_map>

#include "animEng.h"
#include "camera.h"
#include "common.h"
#include "defines.h"
#include "gui.h"
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

template <typename T>
concept Container = requires(T t) {
  std::begin(t);
  std::end(t);
};

template <Container T>
void renderHelper(T const &cont) {
  for (auto const &elem : cont) elem.render();
}

void renderHelper(auto const &elem) { elem.render(); }

void render(const PartManager &PartsManager, const std::unordered_map<RobotParts, Part> &RobotParts, auto const &...part) {
  PartsManager.render(RobotParts);
  (renderHelper(part), ...);
  CheckForErrors("render");
}

template <Container T>
void updateHelper(T &cont, Camera const &camera) {
  for (auto &elem : cont) elem.updateShaders(camera);
}

void updateHelper(auto &elem, Camera const &camera) { elem.updateShaders(camera); }

void updateShaders(PartManager &PartsManager, Camera const &camera, auto &...part) {
  PartsManager.updateShaders(camera);
  (updateHelper(part, camera), ...);
  CheckForErrors("Shader Update");
}

int main() {
  {
    int         Radius = 3;
    GLFWwindow *window = init();
    ImGuiInit();
    Camera camera(Radius, 0, {0.0f, 0.0f, 0.0f});
    Point  endPoint{{1, 1, 1}, {1, 0.2, 0}};
    Gui    gui(ImGui::GetIO(), window);
    try {
      PartManager                          partManger;
      std::unordered_map<RobotParts, Part> Parts;

      make_robot(Parts, partManger);
      while (!glfwWindowShouldClose(window)) {
        preRender();
        gui.newFrame();
        processInput(window, camera, partManger);
        updateShaders(partManger, camera, endPoint);
        render(partManger, Parts, endPoint);
        gui.renderImguiWindow();
        endPoint = gui.CurrentPointPos;

        CheckForErrors("Something went wrong ");
        glfwSwapBuffers(window);
        glfwPollEvents();
      }
    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
    }
  }
  glfwTerminate();
  return 0;
}
