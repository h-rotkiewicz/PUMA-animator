#include "animEng.h"

#include <future>

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
    case RobotParts::joint:
      os << "Joint";
      break;
    case RobotParts::forearm:
      os << "Forearm";
      break;
    case RobotParts::hand:
      os << "Hand";
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

void ShaderManager::updateShader(const Camera &camera, GLFWwindow *window) {
  auto UpdateShader = [&, this](Shader const &S, ModelState const &state) {
    S.use();
    glm::mat4 view(1.0f);
    glm::mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)Settings::SCR_WIDTH / (float)Settings::SCR_HEIGHT, 0.1f, 100.0f);
    view       = glm::lookAt(camera.cameraPosition, camera.getCameraTarget(), camera.getCameraUp());

    S.setMat4("projection", projection);
    S.setMat4("view", view);
    S.setVec3("lightColor", Settings::lightColor);
    S.setVec3("lightPos",
              camera.cameraPosition);  // I think it makes sense for max visibility
    S.setVec3("viewPos", camera.cameraPosition);
    S.setMat4("model", state.model);
  };
  for (auto const &shader : ShaderMap) {
    UpdateShader(shader.second, StateMap.at(shader.first));
  }
}

// WARNING: PAIN AND SUFFERING AHEAD
void ShaderManager::RotatePart(RobotParts part, float angle) {
  constexpr std::array<RobotParts, Settings::RobotSize> PartOrder{RobotParts::base, RobotParts::upper_base, RobotParts::middle_arm, RobotParts::joint, RobotParts::forearm, RobotParts::hand};

  auto       CalledPartPtr          = find_in_array(PartOrder, part);
  auto      &Called_part_state      = StateMap.find(part)->second;
  glm::vec3 &Called_part_pivotPoint = Called_part_state.pivotPoint;
  Called_part_state.angle.add_angle(angle);
  for (auto i = CalledPartPtr; i < PartOrder.end(); i++) {
    auto &part_state = StateMap.at(*i);
    if (*CalledPartPtr == *i) {
      part_state.model = glm::translate(part_state.model, Called_part_pivotPoint);
      part_state.model = glm::rotate(part_state.model, glm::radians(Called_part_state.angle.get_angle_diff()), Called_part_state.axis);
      part_state.model = glm::translate(part_state.model, -Called_part_pivotPoint);
    } else {
      glm::qua  rotation      = glm::angleAxis(glm::radians(Called_part_state.angle.get_angle_diff()), Called_part_state.axis);
      glm::mat4 rotation_mat4 = glm::mat4_cast(rotation);
      glm::mat4 backToPivot   = glm::translate(glm::mat4(1.0f), Called_part_pivotPoint);
      glm::mat4 toOrigin      = glm::translate(glm::mat4(1.0f), -Called_part_pivotPoint);
      glm::mat4 combinedTransform = backToPivot * rotation_mat4 * toOrigin;
      part_state.model            = combinedTransform * part_state.model;
      // part_state.axis             = combinedTransform * glm::vec4(part_state.axis, 1);
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
    state.pivotPoint = {-0.0003, 0.47991, 0.104};
  } else if (part == RobotParts::middle_arm) {
    state.axis       = {0, 0, 1};
    state.pivotPoint = {0.008803, 0.757991, -0.103817};
  } else if (part == RobotParts::joint) {
    state.axis       = {0, 0, 1};
    state.pivotPoint = {-0.003255, 1.57, -0.103817};
  } else if (part == RobotParts::forearm) {
    state.axis       = glm::normalize(glm::vec3(0.32, 0.68, 0));
    state.pivotPoint = {-0.13, 1.326, 0.03};
  } else if (part == RobotParts::hand) {
    state.axis       = glm::normalize(glm::vec3(0.32, 0.68, 0));
    state.pivotPoint = {-0.36, 0.880496, 0.058};
  }
  StateMap.try_emplace(part, state);
}

static float getAnswer() {
  float answer;
  std::cin >> answer;
  return answer;
}

void ShaderManager::render(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) {
  for (const auto &[RobotPart, part] : rendercontainer) {
    ShaderMap.at(RobotPart).use();
    part.Render();
  }
  CheckForErrors("render");
  glfwSwapBuffers(window);
  glfwPollEvents();
}
