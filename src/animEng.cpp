#include "animEng.h"

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "settings.h"

// TODO: Delete later
std::ostream &operator<<(std::ostream &os, const RobotParts &part) {
  switch (part) {
    case RobotParts::base:
      os << "Base";
      break;
    case RobotParts::upper_base:
      os << "Upper Base";
      break;
    case RobotParts::middle_arm:
      os << "Middle Arm";
      break;
    default:
      os << "Unknown";
  }
  return os;
}

constexpr auto find_in_array(auto const &array, auto const &part_) {
  for (int i = 0; i < array.size(); i++)
    if (array[i] == part_) return array.begin() + i;
  throw std::runtime_error("Part not found in Container");
}

void ShaderManager::updateShader(const Camera &camera) {
  auto UpdateShader = [&camera](Shader const &S, ModelState const &state) {
    S.use();
    glm::mat4 view(1.0f);
    glm::mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)Settings::SCR_WIDTH / (float)Settings::SCR_HEIGHT, 0.1f, 100.0f);
    // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    view = glm::lookAt(camera.cameraPosition, camera.getCameraTarget(), camera.getCameraUp());

    S.setMat4("projection", projection);
    S.setMat4("view", view);
    S.setVec3("lightColor", Settings::lightColor);
    S.setVec3("lightPos",
              camera.cameraPosition);  // I think it makes sense for max visibility
    S.setVec3("viewPos", camera.cameraPosition);
    S.setMat4("model", state.model);
  };
  for (auto const &shader : ShaderMap) UpdateShader(shader.second, StateMap.at(shader.first));
}
void rotateModel(glm::mat4& modelMatrix, const glm::vec3& pivotPoint, const glm::quat& rotation) {
    // Extract the current transformation of the model
    glm::mat4 currentTransform = modelMatrix;

    // Translate model so pivot point is at the origin
    glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -pivotPoint);

    // Apply the rotation at the origin
    glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

    // Translate model back to its original position
    glm::mat4 backToPivot = glm::translate(glm::mat4(1.0f), pivotPoint);

    // Combine transformations: first move to pivot point, then apply rotation, then move back
    glm::mat4 combinedTransform = backToPivot * rotationMatrix * toOrigin;

    // Apply the combined transformation to the current model matrix
    modelMatrix = combinedTransform * currentTransform;
}

// WARNING: PAIN AND SUFFERING AHEAD
void ShaderManager::RotatePart(RobotParts part, float angle) {
  constexpr std::array<RobotParts, Settings::RobotSize> PartOrder{RobotParts::base, RobotParts::upper_base, RobotParts::middle_arm};

  auto      CurrentPart        = find_in_array(PartOrder, part);
  auto     &current_part_state = StateMap.find(part)->second;
  glm::vec3 pivotPoint         = current_part_state.pivotPoint;
  current_part_state.angle.add_angle(angle);  // angle is saved in model
  for (auto i = CurrentPart; i < PartOrder.end(); i++) {
    auto &part_state = StateMap.at(*i);
    if (*CurrentPart == *i) {
      part_state.model = glm::translate(part_state.model, pivotPoint);
      part_state.model = glm::rotate(part_state.model, glm::radians(current_part_state.angle.get_angle_diff()), current_part_state.axis);
      part_state.model = glm::translate(part_state.model, -pivotPoint);
    } else {
      rotateModel(part_state.model, pivotPoint, glm::angleAxis(glm::radians(current_part_state.angle.get_angle_diff()), current_part_state.axis));



    }
  }
}

std::unordered_map<RobotParts, Shader> const &ShaderManager::getShaderMap() const { return ShaderMap; }

void ShaderManager::addShader(RobotParts part, Shader &&shader) {
  ShaderMap.emplace(part, std::move(shader));
  ModelState state{.axis = {0, 0, 0}, .pivotPoint{0, 0, 0}, .angle{}};

  // WARNING: Data from blender do not change. conversion from blender to .obj (x,y,z) -> (x,z,y) ?? idk why
  if (part == RobotParts::base) {
    state.axis       = {0, 0, 0};
    state.pivotPoint = {0, 0, 0};
  } else if (part == RobotParts::upper_base) {
    state.axis       = {0, 1, 0};
    state.pivotPoint = {-0.0003, 0, 0.104};
  } else if (part == RobotParts::middle_arm) {
    state.axis       = {0, 0, 1};
    state.pivotPoint = {-0.008803, 0.757991, 0.353817};
  }

  StateMap.try_emplace(part, state);
}
void ShaderManager::render(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) const {
  for (const auto &[RobotPart, part] : rendercontainer) {
    ShaderMap.at(RobotPart).use();
    part.Render();
  }
  CheckForErrors("render");
  glfwSwapBuffers(window);
  glfwPollEvents();
}
