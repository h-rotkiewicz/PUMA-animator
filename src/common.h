#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// clang-format on 
#include <string_view>


enum class RobotParts { base, upper_base, middle_arm, joint, forearm, hand };

struct GLBuffers{
  GLuint VAO{};
  GLuint VBO{}; 
  GLuint EBO{};
  std::size_t ebo_size{};
};

class Part {
  GLBuffers buffers{};
public:
  Part(const Part&) = delete;
  Part &operator=(const Part&) = delete;
  Part(Part&& other) ;
  Part &operator=(Part&& other) ;
  Part(std::string_view path) ;
  void Render() const ;
  ~Part() ;
};

void CheckForErrors(std::string_view message = "");
void framebufferSizeCallback([[maybe_unused]]GLFWwindow *window, int width, int height);
void preRender();
GLFWwindow *init();
GLBuffers bindBuffers(const std::vector<GLuint>& vertices, const std::vector<unsigned int>& indices);
GLBuffers load_vxo(std::string_view path);

