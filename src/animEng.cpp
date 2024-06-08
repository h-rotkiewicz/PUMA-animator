#include "animEng.h"

#include <utility>

#include "defines.h"
#include "settings.h"

constexpr std::array<RobotParts, Settings::RobotSize> PartOrder{RobotParts::base, RobotParts::upper_base, RobotParts::middleArm, RobotParts::joint, RobotParts::forearm, RobotParts::hand};

constexpr auto find_in_array(auto const &array, auto const &part_) {
  for (size_t i = 0; i < array.size(); i++)
    if (array[i] == part_) return array.begin() + i;
  throw std::runtime_error("Part not found in Container");
}

void generateSphere(float radius, int sectorCount, int stackCount, std::vector<glm::vec3> &vertices, std::vector<GLuint> &indices) {
  for (int i = 0; i <= stackCount; ++i) {
    float stackAngle = M_PI / 2 - i * M_PI / stackCount;  // from pi/2 to -pi/2
    float xy         = radius * cosf(stackAngle);         // r * cos(u)
    float z          = radius * sinf(stackAngle);         // r * sin(u)
    for (int j = 0; j <= sectorCount; ++j) {
      float sectorAngle = j * 2 * M_PI / sectorCount;  // from 0 to 2pi
      float x           = xy * cosf(sectorAngle);      // r * cos(u) * cos(v)
      float y           = xy * sinf(sectorAngle);      // r * cos(u) * sin(v)
      vertices.push_back(glm::vec3(x, y, z));
    }
  }
  for (int i = 0; i < stackCount; ++i) {
    int k1 = i * (sectorCount + 1);  // beginning of current stack
    int k2 = k1 + sectorCount + 1;   // beginning of next stack
    for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      if (i != (stackCount - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }
}

void renderPoint(const glm::vec3 &pos, float size = 0.01f) {
  GLuint                 VAO, VBO, EBO;
  std::vector<glm::vec3> vertices;
  std::vector<GLuint>    indices;

  int sectorCount = 36;
  int stackCount  = 18;

  generateSphere(size, sectorCount, stackCount, vertices, indices);

  for (auto &vertex : vertices) vertex += pos;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
  glEnableVertexAttribArray(0);

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

  glDisableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
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

void UpdateShader(const Shader &S, ModelState &state, const Camera &camera) {
  S.use();
  auto projection = glm::perspective(glm::radians(45.0f), static_cast<float>(Settings::Window_Width) / Settings::Window_Height, 0.1f, 100.0f);
  auto view       = glm::lookAt(camera.cameraPosition, camera.getCameraTarget(), camera.getCameraUp());

  S.setMat4("projection", projection);
  S.setMat4("view", view);
  S.setVec3("lightColor", Settings::lightColor);
  S.setVec3("lightPos", camera.cameraPosition);
  S.setVec3("viewPos", camera.cameraPosition);
  S.setMat4("model", state.model);
  S.setInt("texture1", 0);

  state.projection = projection;
  state.view       = view;
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

void PartManager::updateShaders(const Camera &camera) {
  for (auto const &shader : ShaderMap) UpdateShader(shader.second, StateMap.at(shader.first), camera);
}

// Angle in radians
float PartManager::RotatePart(RobotParts part, float angle) {
  if (angle == 0) return StateMap.find(part)->second.angle;
  const auto       CalledPartPtr          = find_in_array(PartOrder, part);
  auto            &Called_part_state      = StateMap.find(part)->second;
  const glm::vec3 &Called_part_pivotPoint = Called_part_state.pivotPoint;
  Called_part_state.angle += angle;
  const glm::qua  rotation          = glm::angleAxis(angle, Called_part_state.axis);
  const glm::mat4 rotation_mat4     = glm::mat4_cast(rotation);
  const glm::mat4 backToPivot       = glm::translate(glm::mat4(1.0f), Called_part_pivotPoint);
  const glm::mat4 toOrigin          = glm::translate(glm::mat4(1.0f), -Called_part_pivotPoint);
  const glm::mat4 combinedTransform = backToPivot * rotation_mat4 * toOrigin;
  for (auto i = CalledPartPtr; i < PartOrder.end(); i++) {
    auto &part_state      = StateMap.at(*i);
    part_state.model      = combinedTransform * part_state.model;
    part_state.pivotPoint = combinedTransform * glm::vec4(part_state.pivotPoint, 1.0f);
    part_state.axis       = glm::normalize(rotation * part_state.axis);
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
  } else if (part == RobotParts::middleArm) {
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

void PartManager::rotateToPoint(glm::vec3 const &endPos) {
  using namespace std;
  using namespace RobotDimensions;

  /* Math and how OpenGl deals with coords have different conventions ergo the weird naming*/
  const auto  x     = endPos.x;
  const auto  y     = -endPos.z;
  const auto  z     = endPos.y;
  const float Speed = 0.05f;

  const float theta1            = (StateMap[RobotParts::upper_base].angle);
  const float theta2            = (StateMap[RobotParts::middleArm].angle);
  const float theta3            = (StateMap[RobotParts::joint].angle);
  const float TargetBaseAngle   = (atan2(y, x));
  const auto  bigDelta          = x * cos(TargetBaseAngle) + y * sin(TargetBaseAngle);
  const auto  omega             = z - L1;
  const auto  delta             = pow(2 * bigDelta * L2, 2) + pow(2 * omega * L2, 2) - pow((pow(bigDelta, 2) + pow(omega, 2) + pow(L2, 2) - pow(L3, 2)), 2);
  const float TargetMiddleAngle = -(atan2(2 * omega * L2 * (bigDelta * bigDelta + omega * omega + L2 * L2 - L3 * L3) + 2 * bigDelta * L2 * sqrt(delta),
                                                      2 * bigDelta * L2 * (bigDelta * bigDelta + omega * omega + L2 * L2 - L3 * L3) - 2 * omega * L2 * sqrt(delta)));
  const float TargetJointAngle  = -(-TargetMiddleAngle + atan2(omega - L2 * sin(TargetMiddleAngle), bigDelta - L2 * cos(TargetMiddleAngle)));

  if (delta < 0) cerr << "No solution" << endl;

  if (!moreOrLess(theta1, TargetBaseAngle,Speed)) {

    std::cout << "Base angle: " << theta1 << std::endl;
    std::cout << "Target Base angle: " << TargetBaseAngle << std::endl;
    RotatePart(RobotParts::upper_base, getRotationDirection(theta1, TargetBaseAngle) * Speed);
  }
  if (!moreOrLess(theta2, TargetMiddleAngle, Speed)) {
    std::cout << "Middle arm angle: " << theta2 << std::endl;
    std::cout << "Target middle angle: " << TargetMiddleAngle << std::endl;
    RotatePart(RobotParts::middleArm, getRotationDirection(theta2, TargetMiddleAngle ) * Speed);
  }
  if (!moreOrLess(theta3, TargetJointAngle, Speed)) {
    std::cout << "Joint angle: " << theta3 << std::endl;
    std::cout << "Target joint angle: " << TargetJointAngle << std::endl;
    RotatePart(RobotParts::joint, getRotationDirection(theta3, TargetJointAngle ) * Speed);
  }
}

void Point::render() const {
  pointshader.use();
  pointshader.setVec3("color", color);
  pointshader.setVec3("center", position);
  pointshader.setFloat("maxDistance", size);
  renderPoint(position, size);
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

Part::Part(std::string_view path) : RenderableObject() { buffers = load_vxo(path); }

RenderableObject::~RenderableObject() {
  glDeleteVertexArrays(1, &buffers.VAO);
  glDeleteBuffers(1, &buffers.VBO);
  glDeleteBuffers(1, &buffers.EBO);
}

RenderableObject &RenderableObject::operator=(RenderableObject &&other) {
  if (this != &other) buffers = std::exchange(other.buffers, GLBuffers{});
  return *this;
};
RenderableObject::RenderableObject(RenderableObject &&other) { buffers = std::exchange(other.buffers, GLBuffers{}); };
void PartManager::zeroRobot() {
  RotatePart(RobotParts::joint, glm::radians(205.f));
  RotatePart(RobotParts::middleArm, glm::radians(90.f));
  StateMap.at(RobotParts::joint).angle = 0;
  StateMap.at(RobotParts::middleArm).angle = 0;
}

