#pragma once
#include "ModelParams.h"
#include "common.h"

template <typename T>
struct OldValComparator{
  T oldVal;
  T newVal;
  void setValue(T newVal){
    this->oldVal = this->newVal;
    this->newVal = newVal;
  }
  bool isnotSame() const {
    return oldVal != newVal;
  }
};

/* all parts start with 0 angle 0 axis and 0 position, that does not mean that thier
 * state is in fact 0, just that the offset from their state defined in the .obj file is 0  etc. */
struct ModelState {
  glm::vec3 axis{};
  glm::vec3 position{};
  float angle{};
  glm::mat4 model = glm::mat4(1.0f);;
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
