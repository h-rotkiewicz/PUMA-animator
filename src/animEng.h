#pragma once
#include "ModelParams.h"
#include "common.h"
#include "settings.h"

/* all parts start with 0 angle 0 axis and 0 position, that does not mean that thier
 * state is in fact 0, just that the offset from their state defined in the .obj file is 0  etc. */
struct ModelState {
    float angle{};
    glm::vec3 axis{};
    glm::vec3 position{};
};

class ShaderManager {
public:
  void updateShader(const Camera &camera);
  void RotatePart(RobotParts part, float angle);
  std::unordered_map<RobotParts, Shader> const &getShaderMap() const;
  void addShader(RobotParts part, Shader &&shader, glm::vec3 axis);

private:
  std::unordered_map<RobotParts, ModelState> StateMap{};
  std::unordered_map<RobotParts, Shader> ShaderMap{};
};
