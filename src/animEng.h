#pragma once
#include "ModelParams.h"
#include "common.h"
#include "settings.h"

/* all parts start with 0 angle 0 axis and 0 position, that does not mean that thier
 * state is in fact 0, just that the offset from their state defined in the .obj file is 0  etc. */
struct ModelState {
  float     angle{};
  glm::vec3 axis{};
  glm::vec3 position{};
  bool isRotating{};
};

/*The Each object should have 2 shaders, one for individual part, meaning a shader that will only draw a 
 * single object, and a shader that will apply the rotation "up the robot" so  the rotation will propagate to the
 * "children" of the object.*/

class ShaderManager {
 public:
  void updateShader(const Camera &camera);
  void RotatePart(RobotParts part, float angle);
  void addShader(RobotParts part, Shader &&shader, glm::vec3 axis);
  void render(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) const;

  std::unordered_map<RobotParts, Shader> const &getShaderMap() const;

 private:
  std::unordered_map<RobotParts, ModelState> StateMap{};
  std::unordered_map<RobotParts, Shader>     ShaderMap{};
};
