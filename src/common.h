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
#include <vector>

#include "OBJ_Loader.h"

enum class RobotParts { base, upper_base, middle_arm, joint, forearm, hand };

class Part {
  GLuint VBO{}, VAO{}, EBO{};
  size_t ebo_size{};
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
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void preRender();
GLFWwindow *init();
std::tuple<GLuint,GLuint,GLuint> bindBuffers(const std::vector<objl::Vertex> &vertices,const std::vector<unsigned int> &indices);
std::tuple<GLuint, GLuint, GLuint, std::size_t> load_vxo(std::string_view path);

