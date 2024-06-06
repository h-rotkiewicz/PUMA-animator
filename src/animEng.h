#pragma once
#include "camera.h"
#include "common.h"
#include "paths.h"
#include "shader.h"

struct ModelState {
  glm::vec3 axis{};
  glm::vec3 pivotPoint{};
  float     angle{};
  glm::mat4 model      = glm::identity<glm::mat4>();
  glm::mat4 projection = glm::identity<glm::mat4>();
  glm::mat4 view       = glm::identity<glm::mat4>();
};

class Point {
 public:
  Point(glm::vec3 const &position_, glm::vec3 const &color_) : color(color_), position(position_) {}
  void render() const;
  void updateShaders(Camera const &camera) ;

 private:
  glm::vec3  color{};
  glm::vec3  position{};
  ModelState state{};
  Shader     pointshader{Paths::shaders_vs_debug, Paths::shaders_fs_debug};
};

class PartManager {
 public:
  void updateShaders(const Camera &camera);
  void RotatePart(RobotParts part, float angle);
  void addShader(RobotParts part, Shader &&shader);
  void render_debug(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) const;
  void render(std::unordered_map<RobotParts, Part> const &rendercontainer) const;

  std::unordered_map<RobotParts, Shader> const &getShaderMap() const;

 private:
  std::unordered_map<RobotParts, ModelState> StateMap{};
  std::unordered_map<RobotParts, Shader>     ShaderMap{};

  Shader DebugShader{Paths::shaders_vs_debug, Paths::shaders_fs_debug};
};
