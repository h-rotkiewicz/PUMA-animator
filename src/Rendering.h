#pragma once
#include "main.h"

struct Settings {
  // settings
  constexpr static unsigned int SCR_WIDTH = 800;
  constexpr static unsigned int SCR_HEIGHT = 600;
};
template<typename Destructor >
class bufferWrapper {
public:
  ~bufferWrapper(){
  };

  GLuint buffer{};
};

class Part {
  bufferWrapper<decltype(glDeleteBuffers)> VBO, EBO;
  bufferWrapper<decltype(glDeleteVertexArrays)> VAO;
  std::vector<objl::Vertex> vertices{};
  std::vector<unsigned int> indices{};
public:
  Part(std::string_view obj_path);
  void Render() const;
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
  for (auto const &part : rendercontainer) {
    part.Render();
  }
  CheckForErrors("render");
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void make_robot(auto & container){
  container.emplace_back(Paths::resources_base_obj.string());
  container.emplace_back(Paths::resources_upper_base_obj.string());
  container.emplace_back(Paths::resources_middle_arm_obj.string());
}
