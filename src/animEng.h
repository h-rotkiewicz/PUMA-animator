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

class RenderableObject {
 protected:
  GLBuffers buffers{};
  RenderableObject()                                    = default;
  RenderableObject(const RenderableObject &)            = delete;  // Not necessary for now
  RenderableObject &operator=(const RenderableObject &) = delete;
  RenderableObject(RenderableObject &&other);
  RenderableObject &operator=(RenderableObject &&other);
  ~RenderableObject();
};

class Part : public RenderableObject {
 public:
  Part(std::string_view path);
  void Render() const;
};

class Point : public RenderableObject {
 public:
  Point(glm::vec3 const &position_, glm::vec3 const &color_) : RenderableObject(), color(color_), position(position_) {}
  void      render() const;
  void      updateShaders(Camera const &camera);
  void      operator=(glm::vec3 const &newpos);
  glm::vec3 getPosition() const { return position; }

 private:
  glm::vec3  color{};
  glm::vec3  position{};
  float      size = 0.04f;
  ModelState state{};
  Shader     pointshader{Paths::shaders_vs_debug, Paths::shaders_point_fs};
};

namespace RobotDimensions {
constexpr float L1 = 0.77991;
constexpr float L2 = 0.834;
constexpr float L3 = 1.238;
};  // namespace RobotDimensions


class PartManager {
 public:
  void  updateShaders(const Camera &camera);
  float RotatePart(RobotParts part, float angle = 0.05f);
  void  zeroRobot();

  void render_debug(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) const;
  void render(std::unordered_map<RobotParts, Part> const &rendercontainer) const;
  void addShader(RobotParts part, Shader &&shader);
  void rotateToPoint(glm::vec3 const &endPos);

  std::unordered_map<RobotParts, Shader> const &getShaderMap() const;

 private:
  std::unordered_map<RobotParts, ModelState> StateMap{};
  std::unordered_map<RobotParts, Shader>     ShaderMap{};

  Shader DebugShader{Paths::shaders_vs_debug, Paths::shaders_fs_debug};
};
