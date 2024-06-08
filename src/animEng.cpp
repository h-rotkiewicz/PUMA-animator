#include "animEng.h"

#include <utility>

#include "settings.h"

constexpr std::array<RobotParts, Settings::RobotSize> PartOrder{RobotParts::base, RobotParts::upper_base, RobotParts::middle_arm, RobotParts::joint, RobotParts::forearm, RobotParts::hand};
constexpr auto                                        find_in_array(auto const &array, auto const &part_) {
  for (size_t i = 0; i < array.size(); i++)
    if (array[i] == part_) return array.begin() + i;
  throw std::runtime_error("Part not found in Container");
}

auto UpdateShader = [](Shader const &S, ModelState &state, Camera const &camera) {
  S.use();
  auto projection = glm::perspective(glm::radians(45.0f), (float)Settings::Window_Width / (float)Settings::Window_Height, 0.1f, 100.0f);
  auto view       = glm::lookAt(camera.cameraPosition, camera.getCameraTarget(), camera.getCameraUp());

  S.setMat4("projection", projection);
  S.setMat4("view", view);
  S.setVec3("lightColor", Settings::lightColor);
  S.setVec3("lightPos",
            camera.cameraPosition);  // I think it makes sense for max visibility
  S.setVec3("viewPos", camera.cameraPosition);
  S.setMat4("model", state.model);
  glUniform1i(glGetUniformLocation(S.ID, "texture1"), 0);
  S.setInt("texture1", 0);
  state.projection = projection;
  state.view       = view;
};

void PartManager::updateShaders(const Camera &camera) {
  for (auto const &shader : ShaderMap) UpdateShader(shader.second, StateMap.at(shader.first), camera);
}


//Angle in radians
float PartManager::RotatePart(RobotParts part, float angle) {
  if (angle == 0) return StateMap.find(part)->second.angle;
  const auto       CalledPartPtr          = find_in_array(PartOrder, part);
  auto            &Called_part_state      = StateMap.find(part)->second;
  const glm::vec3 &Called_part_pivotPoint = Called_part_state.pivotPoint;
  Called_part_state.angle += angle;
  for (auto i = CalledPartPtr; i < PartOrder.end(); i++) {
    auto           &part_state        = StateMap.at(*i);
    const glm::qua  rotation          = glm::angleAxis(angle, Called_part_state.axis);
    const glm::mat4 rotation_mat4     = glm::mat4_cast(rotation);
    const glm::mat4 backToPivot       = glm::translate(glm::mat4(1.0f), Called_part_pivotPoint);
    const glm::mat4 toOrigin          = glm::translate(glm::mat4(1.0f), -Called_part_pivotPoint);
    const glm::mat4 combinedTransform = backToPivot * rotation_mat4 * toOrigin;
    part_state.model                  = combinedTransform * part_state.model;
    part_state.pivotPoint             = combinedTransform * glm::vec4(part_state.pivotPoint, 1.0f);
    part_state.axis                   = glm::normalize(rotation * part_state.axis);
  }
  return Called_part_state.angle;
}

std::unordered_map<RobotParts, Shader> const &PartManager::getShaderMap() const { return ShaderMap; }

void PartManager::addShader(RobotParts part, Shader &&shader) {
  ShaderMap.emplace(part, std::move(shader));
  ModelState state{.axis = {0, 0, 0}, .pivotPoint{0, 0, 0}, .angle{}};

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

void renderPoint(const glm::vec3 &pivotPoint) {
  GLuint VAO, VBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &pivotPoint, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
  glEnableVertexAttribArray(0);

  glPointSize(20.0f);
  glDrawArrays(GL_POINTS, 0, 1);

  glDisableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}

void renderLine(const glm::vec3 &start, const glm::vec3 &end) {
  GLuint VAO, VBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  const std::array Line = {start, start + end};

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Line), &Line, GL_STATIC_DRAW);

  // Vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

  // Draw the line
  glDrawArrays(GL_LINES, 0, 2);

  // Unbind and cleanup
  glDisableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}

void PartManager::render_debug(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) const {
  for (const auto &[RobotPart, part] : rendercontainer) {
    ShaderMap.at(RobotPart).use();
    part.Render();
  }
  CheckForErrors("render");
  DebugShader.use();
  for (auto const &[part, state] : StateMap) {
    DebugShader.setMat4("model", state.model);
    DebugShader.setMat4("view", state.view);
    DebugShader.setMat4("projection", state.projection);
    DebugShader.setVec3("color", {1, 0, 0});  // Red for PivotPoints
    renderPoint(state.pivotPoint);
    DebugShader.setVec3("color", {0, 1, 0});  // Green for AxisPoints
    renderPoint(state.axis + state.pivotPoint);
    DebugShader.setVec3("color", {0, 0, 1});  // blue for AxisLines
    renderLine(state.pivotPoint, state.axis);
  }
  CheckForErrors("Debugrender");
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void PartManager::render(std::unordered_map<RobotParts, Part> const &rendercontainer) const {
  for (const auto &[RobotPart, part] : rendercontainer) {
    ShaderMap.at(RobotPart).use();
    part.Render();
  }
}

void Point::render() const {
  pointshader.use();
  pointshader.setVec3("color", color);
  renderPoint(position);
  renderLine({0, 0, 0}, position);
}

void Point::updateShaders(Camera const &camera) { UpdateShader(pointshader, state, camera); }
void Point::operator=(glm::vec3 const &newpos) { position = newpos; }

void Part::Render() const {
  glBindVertexArray(buffers.VAO);
  CheckForErrors(std::string("Binding VAO: ") + std::to_string(buffers.VAO));
  glDrawElements(GL_TRIANGLE_STRIP,
                 buffers.ebo_size,
                 GL_UNSIGNED_INT,
                 0);  // traiangle strip Works better ???? why ??
                      // GL_LINES_ADJACENCY also looks cool
                      // GL_LINES also looks cool
  CheckForErrors("Drawing elements");
}

Part::~Part() {
  glDeleteVertexArrays(1, &buffers.VAO);
  glDeleteBuffers(1, &buffers.VBO);
  glDeleteBuffers(1, &buffers.EBO);
}

Part::Part(std::string_view path) { buffers = load_vxo(path); }

Part &Part::operator=(Part &&other) {
  buffers.VAO      = std::exchange(other.buffers.VAO, 0);
  buffers.VBO      = std::exchange(other.buffers.VBO, 0);
  buffers.EBO      = std::exchange(other.buffers.EBO, 0);
  buffers.ebo_size = std::exchange(other.buffers.ebo_size, 0);
  return *this;
}

Part::Part(Part &&other) {
  buffers.VAO      = std::exchange(other.buffers.VAO, 0);
  buffers.VBO      = std::exchange(other.buffers.VBO, 0);
  buffers.EBO      = std::exchange(other.buffers.EBO, 0);
  buffers.ebo_size = std::exchange(other.buffers.ebo_size, 0);
}
void PartManager::rotateToPoint(glm::vec3 const &endPos) {
  using namespace std;
  using namespace RobotDimensions;

  /* Math and how OpenGl deals with coords have different conventions ergo the weird naming*/
  const auto  x     = endPos.x;
  const auto  y     = -endPos.z;
  const auto  z     = endPos.y;
  const float Speed = 0.05f;

  const float theta1            = (StateMap[RobotParts::upper_base].angle);
  const float theta2            = (StateMap[RobotParts::middle_arm].angle);
  const float theta3            = (StateMap[RobotParts::joint].angle);
  const float TargetBaseAngle   = (atan2(y, x));
  const auto  bigDelta          = x * cos(TargetBaseAngle) + y * sin(TargetBaseAngle);
  const auto  omega             = z - L1;
  const auto  delta             = pow(2 * bigDelta * L2, 2) + pow(2 * omega * L2, 2) - pow((pow(bigDelta, 2) + pow(omega, 2) + pow(L2, 2) - pow(L3, 2)), 2);
  const float TargetMiddleAngle = (atan2(2 * omega * L2 * (bigDelta * bigDelta + omega * omega + L2 * L2 - L3 * L3) - 2 * bigDelta * L2 * sqrt(delta),
                                                      2 * bigDelta * L2 * (bigDelta * bigDelta + omega * omega + L2 * L2 - L3 * L3) + 2 * omega * L2 * sqrt(delta)));
  const float TargetJointAngle  = (-TargetMiddleAngle + atan2(omega - L2 * sin(TargetMiddleAngle), bigDelta - L2 * cos(TargetMiddleAngle)));

  if (delta < 0) cerr << "No solution" << endl;

  if (!moreOrLess(theta1, TargetBaseAngle,Speed)) {
    std::cout << "Base angle: " << theta2 << std::endl;
    std::cout << "Target Base angle: " << TargetMiddleAngle << std::endl;
    RotatePart(RobotParts::upper_base, getRotationDirection(theta1, TargetBaseAngle) * Speed);
  }
  if (!moreOrLess(theta2, TargetMiddleAngle,Speed)) {
    std::cout << "Middle arm angle: " << theta2 << std::endl;
    std::cout << "Target middle angle: " << TargetMiddleAngle << std::endl;
    RotatePart(RobotParts::middle_arm, getRotationDirection(theta2, TargetMiddleAngle) * Speed);
  }
  if (!moreOrLess(theta3, TargetJointAngle, Speed)) {
    std::cout << "Joint angle: " << theta3 << std::endl;
    std::cout << "Target joint angle: " << TargetJointAngle << std::endl;
    RotatePart(RobotParts::joint, getRotationDirection(theta3, TargetJointAngle) * Speed);
  }
}

