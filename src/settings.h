#pragma once
#include "glm/ext/vector_float3.hpp"
struct Settings {
  constexpr static unsigned int Window_Width = 800;
  constexpr static unsigned int Window_Height = 600;
  constexpr static glm::vec3 lightColor{ 1.f, 1.f, 1.f };
  constexpr static size_t RobotSize = 6;
};
