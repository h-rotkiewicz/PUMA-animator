#pragma once
#include <glm/glm.hpp>
struct ModelParams {
  float angle{};
  glm::vec3 RotationAxis = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 modelPosition = glm::vec3(0.0f, -0.5f, 0.0f);
  glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 cameraTarget = modelPosition;
  glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
};
