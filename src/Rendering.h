#pragma once
#include "ModelParams.h"
#include "common.h"
#include "paths.h"
#include "shader.h"
#include <unordered_map>

struct Settings {
  // settings
  constexpr static unsigned int SCR_WIDTH = 800;
  constexpr static unsigned int SCR_HEIGHT = 600;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, Camera &camera);
GLFWwindow *init();
std::tuple<unsigned int, unsigned int, unsigned int>
bindBuffers(const std::vector<objl::Vertex> &vertices,
            const std::vector<unsigned int> &indices);
GLuint bindTexture(std::string_view texturePath);
void preRender();
void updateShader(const Shader &Shader, const ModelParams &modelParams,
                  const Camera &camera);
void render(auto const &rendercontainer,
            GLFWwindow *window) { // auto bc I am lazy and I change
                                  // my mind a lot
  for (const auto &[_, part] : rendercontainer) {
    std::visit(Render, part);
  }
  CheckForErrors("render");
  glfwSwapBuffers(window);
  glfwPollEvents();
}

std::tuple<GLuint, GLuint, GLuint> load_vxo(std::string_view path,
                                            std::vector<objl::Vertex> &vertices,
                                            std::vector<unsigned int> &indices);

template <typename CallBack>
auto make_part(std::string_view path) {
    std::vector<objl::Vertex> vertices;
    std::vector<unsigned int> indices;
    auto [VAO, VBO, EBO] = load_vxo(path, vertices, indices);
    return Part<CallBack>(VAO, VBO, EBO, std::move(vertices), std::move(indices));
}
/* I feel as tho I need to explain myself, so... why is this so fucked up ?
 * I tried to avoid using runtime polymorphism
 * */
inline void make_robot(std::unordered_map<RobotParts, PartType> &container) {
  objl::Loader Loader;
  container.insert({RobotParts::base, make_part<decltype(no_rotation)>(Paths::resources_base_obj.string())});
  container.insert({RobotParts::upper_base, make_part<decltype(base_rotate)>(Paths::resources_upper_base_obj.string())});
  container.insert({RobotParts::middle_arm, make_part<decltype(arm_rotate)>(Paths::resources_middle_arm_obj.string())});
}
