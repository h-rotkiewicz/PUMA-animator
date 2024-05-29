#include "animEng.h"

#include <algorithm>

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
  auto UpdateShader = [&camera](Shader &S, ModelState const &state) {
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
  for (auto &shader : ShaderMap) UpdateShader(shader.second, StateMap.at(shader.first));
}

void ShaderManager::RotatePart(RobotParts part, float angle) {
  constexpr std::array<RobotParts, Settings::RobotSize> PartOrder{RobotParts::base, RobotParts::upper_base, RobotParts::middle_arm};

  auto  CurrentPart        = find_in_array(PartOrder, part);
  auto &current_part_state = StateMap.find(part)->second;

  for (auto i = CurrentPart; i < PartOrder.end(); i++) {
    // std::cout << "Rotating: " << *i << " by " << angle << " on axis: " << axis.x << axis.y << axis.z<< std::endl;
    StateMap.at(*i).angle = angle;
    StateMap.at(*i).model = glm::rotate(StateMap.at(*i).model, glm::radians(current_part_state.angle), current_part_state.axis);
  }
}

std::unordered_map<RobotParts, Shader> const &ShaderManager::getShaderMap() const { return ShaderMap; }

void ShaderManager::addShader(RobotParts part, Shader &&shader) {
  ShaderMap.emplace(part, std::move(shader));
  ModelState state{.axis = {0, 0, 0}, .position{0, 0, 0}, .angle = 0};

  if (part == RobotParts::base)
    state.axis = {0, 0, 0};
  else if (part == RobotParts::upper_base)
    state.axis = {0, 1, 0};
  else if (part == RobotParts::middle_arm)
    state.axis = {0, 0, 1};

  StateMap.emplace(part, state);
}
/* std::for_each(rendercontainer.find(RobotPart), rendercontainer.end(), [&shaderMap = ShaderMap, &StateMap = StateMap, RobotPart](auto const &Elem) {
  if (StateMap.at(RobotPart).Rotation.isRotating()) shaderMap.at(RobotPart).use();
  Elem.second.Render();
}); */
void ShaderManager::render(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) const {
  for (const auto &[RobotPart, part] : rendercontainer) {
    ShaderMap.at(RobotPart).use();
    part.Render();
  }
  CheckForErrors("render");
  glfwSwapBuffers(window);
  glfwPollEvents();
}
