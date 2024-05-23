/* Why does this file exist ? well because  clang-format gets a stroke if I
 * include clang-format off and clang-format on in the main file and refuses to
 * format the code properly. So I put all the includes in this file and include
 * this file  */
#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// clang-format on 

#include "OBJ_Loader.h"
#include "shader.h"
#include <iostream>
#include <vector>

inline void CheckForErrors(std::string_view message = "") {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    std::cerr << message << " || OpenGL Error: " << error << std::endl;
  }
}


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


inline std::tuple<GLuint, GLuint, GLuint> load_vxo(std::string_view path,
                                            std::vector<objl::Vertex> &vertices,
                                            std::vector<unsigned int> &indices){
  objl::Loader Loader;
  if (!Loader.LoadFile(path.data())) {
    std::cerr << "Failed to load OBJ file."<< path.data() << std::endl;
    throw std::runtime_error("Failed to load OBJ file.");

  }
  vertices = Loader.LoadedVertices;
  indices = Loader.LoadedIndices;
  return bindBuffers(vertices, indices);
}

enum class RobotParts { base, upper_base, middle_arm };

template <typename Destructor> class bufferWrapper {
public:
  ~bufferWrapper(){};

  GLuint buffer{};
};


class Part {
 bufferWrapper<decltype(glDeleteBuffers)> VBO, EBO;
 bufferWrapper<decltype(glDeleteVertexArrays)> VAO;
  std::vector<objl::Vertex> vertices{};
  std::vector<unsigned int> indices{};
public:
  Part(std::string_view path) {
    auto [VAO_, VBO_, EBO_] = load_vxo(path, vertices, indices);
    VAO.buffer = VAO_;
    VBO.buffer = VBO_;
    EBO.buffer = EBO_;
  }

void Render(Shader const & shader) const {
    shader.use();
    glBindVertexArray(VAO.buffer);
    CheckForErrors(std::string("Binding VAO: ") + std::to_string(VAO.buffer));
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT,
                   0); // traiangle strip Works better ???? why ??
                       // GL_LINES_ADJACENCY also looks cool
                       // GL_LINES also looks cool
    CheckForErrors("Drawing elements");
  }

};

