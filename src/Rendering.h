#pragma once
#include "ModelParams.h"
#include "common.h"
#include "paths.h"
#include "shader.h"
#include <unordered_map>


struct Settings {
  // settings
  constexpr static unsigned int SCR_WIDTH = 800;
  constexpr static unsigned int SCR_HEIGHT = 600;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, Camera &camera, auto & Parts);

void processInput(GLFWwindow *window, Camera &camera, auto  &Parts) {
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
  if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    camera.change_radius(0.01);
  if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    camera.change_radius(-0.01);
}

GLFWwindow *init();
std::tuple<unsigned int, unsigned int, unsigned int>
bindBuffers(const std::vector<objl::Vertex> &vertices,
            const std::vector<unsigned int> &indices);
GLuint bindTexture(std::string_view texturePath);
void preRender();
void updateShader(const Shader &Shader, const ModelParams &modelParams,
                  const Camera &camera);

void render(auto const &rendercontainer,
            GLFWwindow *window) { // auto bc I am lazy and I change
                                  // my mind a lot
  for (const auto &[_, part] : rendercontainer) {
    part.Render();
  }
  CheckForErrors("render");
  glfwSwapBuffers(window);
  glfwPollEvents();
}



inline void make_robot(std::unordered_map<RobotParts, Part> &container) {
  Shader shader(Paths::shaders_vs.c_str(), Paths::shaders_fs.c_str());
  Shader shader1(Paths::shaders_vs.c_str(), Paths::shaders_fs.c_str());
  Shader shader2(Paths::shaders_vs.c_str(), Paths::shaders_fs.c_str());
  container.emplace(RobotParts::base, Part(Paths::resources_base_obj.string(), std::move(shader)));
  container.emplace(RobotParts::upper_base, Part(Paths::resources_upper_base_obj.string(), std::move(shader1)));
  container.emplace(RobotParts::middle_arm, Part(Paths::resources_middle_arm_obj.string(), std::move(shader2)));
  //debug
  for (auto &[name, part] : container) {
    std::cout << "Part: " << static_cast<int>(name) ;
    std::cout << " Shader ID: " << part.getShader().ID << std::endl;
  }
}
