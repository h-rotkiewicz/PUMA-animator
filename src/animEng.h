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

class Part {
  GLBuffers buffers{};

 public:
  Part(const Part &)            = delete;
  Part &operator=(const Part &) = delete;
  Part(Part &&other);
  Part &operator=(Part &&other);
  Part(std::string_view path);
  void Render() const;
  ~Part();
};

class Point {
 public:
  Point(glm::vec3 const &position_, glm::vec3 const &color_) : color(color_), position(position_) {}
  void      render() const;
  void      updateShaders(Camera const &camera);
  void      operator=(glm::vec3 const &newpos);
  glm::vec3 getPosition() const { return position; }

 private:
  glm::vec3  color{};
  glm::vec3  position{};
  ModelState state{};
  Shader     pointshader{Paths::shaders_vs_debug, Paths::shaders_fs_debug};
};

inline bool moreOrLess(float a, float b, float errorMargin = 1.f) { return std::abs(a - b) <= errorMargin; }

namespace RobotDimensions {
constexpr float L1 = 0.47991;
constexpr float L2 = 0.9;
constexpr float L3 = 1.1;
};  // namespace RobotDimensions

inline int getRotationDirection(float current, float target) {
  if (current < target) return 1;
  if (current > target) return -1;
  return 0;
}

class PartManager {
 public:
  void  updateShaders(const Camera &camera);
  float RotatePart(RobotParts part, float angle);
  void  render_debug(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) const;
  void  render(std::unordered_map<RobotParts, Part> const &rendercontainer) const;
  void  addShader(RobotParts part, Shader &&shader);
  void  rotateToPoint(glm::vec3 const &endPos) {
    using namespace std;
    using namespace RobotDimensions;

    /* Math and how OpenGl deals with coords have different conventions ergo the weird naming*/
    const auto x = endPos.x;
    const auto y = -endPos.z;
    const auto z = endPos.y;

    const float theta1            = (StateMap[RobotParts::upper_base].angle);
    const float theta2            = (StateMap[RobotParts::middle_arm].angle);
    const float theta3            = (StateMap[RobotParts::joint].angle);
    const float TargetBaseAngle   = (glm::degrees(atan2(y, x)));
    const auto  bigDelta          = x * cos(glm::radians(TargetBaseAngle)) + y * sin(glm::radians(TargetBaseAngle));
    const auto  omega             = z - L1;
    const auto  delta             = pow(2 * bigDelta * L2, 2) + pow(2 * omega * L2, 2) - pow((pow(bigDelta, 2) + pow(omega, 2) + pow(L2, 2) - pow(L3, 2)), 2);
    const float TargetMiddleAngle = (glm::degrees(atan2(2 * omega * L2 * (bigDelta * bigDelta + omega * omega + L2 * L2 - L3 * L3) - 2 * bigDelta * L2 * sqrt(delta),
                                                                      2 * omega * L2 * (bigDelta * bigDelta + omega * omega + L2 * L2 - L3 * L3) + 2 * bigDelta * L2 * sqrt(delta))));
    const float TargetJointAngle  = (glm::degrees(-TargetMiddleAngle + atan2(omega - L2 * sin(glm::radians(TargetMiddleAngle)), bigDelta - L2 * cos(glm::radians(TargetMiddleAngle)))));

    if (delta < 0) cerr << "No solution" << endl;

    if (!moreOrLess(theta1, TargetBaseAngle)) {
      std::cout << "Base angle: " << theta2 << std::endl;
      std::cout << "Target Base angle: " << TargetMiddleAngle << std::endl;
      RotatePart(RobotParts::upper_base, getRotationDirection(theta1, TargetBaseAngle) * 1.0f);
    }
    if (!moreOrLess(theta2, TargetMiddleAngle)) {
      std::cout << "Middle arm angle: " << theta2 << std::endl;
      std::cout << "Target middle angle: " << TargetMiddleAngle << std::endl;
      RotatePart(RobotParts::middle_arm, getRotationDirection(theta2, TargetMiddleAngle) * 1.0f);
    }
    if (!moreOrLess(theta3, TargetJointAngle)) {
      std::cout << "Joint angle: " << theta2 << std::endl;
      std::cout << "Target joint angle: " << TargetMiddleAngle << std::endl;
      RotatePart(RobotParts::joint, getRotationDirection(theta3, TargetJointAngle) * 1.0f);
    }
  }

  std::unordered_map<RobotParts, Shader> const &getShaderMap() const;

 private:
  std::unordered_map<RobotParts, ModelState> StateMap{};
  std::unordered_map<RobotParts, Shader>     ShaderMap{};

  Shader DebugShader{Paths::shaders_vs_debug, Paths::shaders_fs_debug};
};
