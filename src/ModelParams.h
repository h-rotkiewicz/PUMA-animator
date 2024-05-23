#pragma once
#include <glm/ext/quaternion_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
  Camera(int Radius, int vert_offset, glm::vec3 const & Target) : cameraPosition{0.0f, 0.0f + vert_offset, Radius}, cameraTarget{Target} {}

  glm::vec3 getCameraTarget() const { return cameraTarget; }
  glm::vec3 getCameraUp() const { return cameraUp; }
  void Orbit(float angle, glm::vec3 const & axis) {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    glm::vec4 newPosition = rotation * glm::vec4(cameraPosition, 1.0f);
    cameraPosition = glm::vec3(newPosition);
  }
  glm::vec3 cameraPosition;

  void change_vert_offset(float change) { cameraPosition.y += change; cameraTarget.y += change; }
  void change_radius(float change) { cameraPosition.z += change; }

  private:

  glm::vec3 cameraTarget;
  glm::vec3 cameraUp{0.0f, 1.0f, 0.0f };

};
