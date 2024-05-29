#pragma once
#include "ModelParams.h"
#include "common.h"

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
  glm::mat4 model = glm::identity<glm::mat4>();
};

/*The Each object should have 2 shaders, one for individual part, meaning a shader that will only draw a
 * single object, and a shader that will apply the rotation "up the robot" so  the rotation will propagate to the
 * "children" of the object.*/
class ShaderManager {
 public:
  void updateShader(const Camera &camera);
  void RotatePart(RobotParts part, float angle);
  void addShader(RobotParts part, Shader &&shader);
  void render(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) const;

  std::unordered_map<RobotParts, Shader> const &getShaderMap() const;

 private:
  std::unordered_map<RobotParts, ModelState> StateMap{};
  std::unordered_map<RobotParts, Shader>     ShaderMap{};
};
