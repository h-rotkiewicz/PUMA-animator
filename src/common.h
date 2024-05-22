/* Why does this file exist ? well because  clang-format gets a stroke if I include clang-format off and clang-format on in the main file 
 * and refuses to format the code properly. So I put all the includes in this file and include this file  */
#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// clang-format on 

#include <iostream>
#include <variant>
#include <vector>
#include "OBJ_Loader.h"

inline void CheckForErrors(std::string_view message = "") {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    std::cerr << message << " || OpenGL Error: " << error << std::endl;
  }
}

inline auto arm_rotate = [](float angle) {
  // TODO: implement this
  std::cout << " Rotating arm by " << angle << std::endl;
};

inline auto base_rotate = [](float angle) {
  // TODO: implement this
  std::cout << " Rotating base by " << angle << std::endl;
};

inline auto no_rotation = [](float angle) {
  std::cout << " No rotation " << std::endl;
};

inline auto Render = [](auto const &part) { part.Render(); };

enum class RobotParts { base, upper_base, middle_arm };

template <typename Destructor> class bufferWrapper {
public:
  ~bufferWrapper(){};

  GLuint buffer{};
};

std::tuple<GLuint, GLuint, GLuint>
bindBuffers(const std::vector<objl::Vertex> &vertices,
            const std::vector<unsigned int> &indices);


template <typename RotateCallback> class Part {
  bufferWrapper<decltype(glDeleteBuffers)> VBO, EBO;
  bufferWrapper<decltype(glDeleteVertexArrays)> VAO;
  std::vector<objl::Vertex> vertices{};
  std::vector<unsigned int> indices{};


public:
  Part( GLuint VAO, GLuint VBO, GLuint EBO, std::vector<objl::Vertex> && vert,  std::vector<unsigned int> && ind) : VAO{VAO}, VBO{VBO}, EBO{EBO} , vertices(std::move(vert)), indices(std::move(ind)){}

  void rotate(float angle) { RotateCallback()(angle); }

  void Render() const {
    glBindVertexArray(VAO.buffer);
    CheckForErrors(std::string("Binding VAO: ") + std::to_string(VAO.buffer));
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT,
                   0); // traiangle strip Works better ???? why ??
                       // GL_LINES_ADJACENCY also looks cool
                       // GL_LINES also looks cool
    CheckForErrors("Drawing elements");
  }
};

using PartType =
    std::variant<Part<decltype(arm_rotate)>, Part<decltype(base_rotate)>,
                 Part<decltype(no_rotation)>>;


