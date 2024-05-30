#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// clang-format on 

#include <iostream>
#include <utility>
#include <vector>

#include "OBJ_Loader.h"

inline void CheckForErrors(std::string_view message = "") {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    std::cerr << message << " || OpenGL Error: " << error << std::endl;
  }
}

enum class RobotParts { base, upper_base, middle_arm };

std::tuple<GLuint,GLuint,GLuint>
inline bindBuffers(const std::vector<objl::Vertex> &vertices,
            const std::vector<unsigned int> &indices) {
 GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(objl::Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);
  CheckForErrors("bindBuffers");

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)offsetof(objl::Vertex, Position));
  glEnableVertexAttribArray(0);
  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)offsetof(objl::Vertex, Normal));
  glEnableVertexAttribArray(1);
  // Tex coordinate attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)offsetof(objl::Vertex, TextureCoordinate));
  glEnableVertexAttribArray(2);
  CheckForErrors("Attributes");

  glBindVertexArray(0); // Unbind VAO
  return {VAO, VBO, EBO};
}


inline std::tuple<GLuint, GLuint, GLuint, std::size_t> load_vxo(std::string_view path){
  objl::Loader Loader;
  if (!Loader.LoadFile(path.data())) {
    std::cerr << "Failed to load OBJ file."<< path.data() << std::endl;
    throw std::runtime_error("Failed to load OBJ file.");

  }
  auto indices = Loader.LoadedIndices;
  auto [VAO, VBO, EBO ] = bindBuffers(Loader.LoadedVertices, indices);
  return {VAO, VBO, EBO, indices.size()};
}




class Part {
GLuint VBO{}, VAO{}, EBO{};
  size_t ebo_size{};
public:
  Part(const Part&) = delete;
  Part &operator=(const Part&) = delete;
  Part(Part&& other) {
    VAO = std::exchange(other.VAO, 0);
    VBO = std::exchange(other.VBO, 0);
    EBO = std::exchange(other.EBO, 0);
    ebo_size = std::exchange(other.ebo_size, 0);
  }
  Part &operator=(Part&& other) {
    VAO = std::exchange(other.VAO, 0);
    VBO = std::exchange(other.VBO, 0);
    EBO = std::exchange(other.EBO, 0);
    ebo_size = std::exchange(other.ebo_size, 0);
    return *this;
  }
Part(std::string_view path) {
    auto [VAO_, VBO_, EBO_, Size] = load_vxo(path);
    VAO = VAO_;
    VBO = VBO_;
    EBO = EBO_;
    ebo_size = Size;
  }

void Render() const {
    glBindVertexArray(VAO);
    CheckForErrors(std::string("Binding VAO: ") + std::to_string(VAO));
    glDrawElements(GL_TRIANGLE_STRIP, ebo_size, GL_UNSIGNED_INT,
                   0); // traiangle strip Works better ???? why ??
                       // GL_LINES_ADJACENCY also looks cool
                       // GL_LINES also looks cool
    CheckForErrors("Drawing elements");
  }
  ~Part() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }
};

