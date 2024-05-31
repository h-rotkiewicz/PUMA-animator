#pragma once
#include "camera.h"
#include "common.h"
#include "paths.h"
#include "shader.h"

//
// Helper struct, for doing reverse kinematics
struct Angle {
 private:
  float old_angle{};
  float new_angle{};

 public:
  auto get_old_angle() const { return old_angle; }
  auto get_new_angle() const { return new_angle; }

  auto get_angle_diff() const { return new_angle - old_angle; }

  auto add_angle(float angle) {
    old_angle = new_angle;
    new_angle += angle;
  }
};

struct ModelState {
  glm::vec3 axis{};
  glm::vec3 pivotPoint{};
  Angle     angle{};
  glm::mat4 model      = glm::identity<glm::mat4>();
  glm::mat4 projection = glm::identity<glm::mat4>();
  glm::mat4 view       = glm::identity<glm::mat4>();
};

class PartManager {
 public:
  void updateShaders(const Camera &camera, GLFWwindow *window);
  void RotatePart(RobotParts part, float angle);
  void addShader(RobotParts part, Shader &&shader);

  void render_debug(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) const;
  void render(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) const;

  std::unordered_map<RobotParts, Shader> const &getShaderMap() const;

 private:
  std::unordered_map<RobotParts, ModelState> StateMap{};
  std::unordered_map<RobotParts, Shader>     ShaderMap{};

  Shader DebugShader{Paths::shaders_vs_debug, Paths::shaders_fs_debug};
};
