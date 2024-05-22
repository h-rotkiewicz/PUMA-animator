#pragma once
#include "main.h"

struct Settings {
  // settings
  constexpr static unsigned int SCR_WIDTH = 800;
  constexpr static unsigned int SCR_HEIGHT = 600;
};


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, Camera &camera);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
GLFWwindow *init();
std::tuple<unsigned int, unsigned int, unsigned int>
bindBuffers(const std::vector<objl::Vertex> &vertices,
            const std::vector<unsigned int> &indices);
GLuint bindTexture( std::string_view texturePath);
void preRender( GLFWwindow *window, const GLuint & texture1, ModelParams &modelParams);
void updateShader(Shader &Shader, const ModelParams &modelParams, const Camera &camera);

void render(const GLuint &VAO, const auto &indices, GLFWwindow * window) { //auto bc I am lazy and I change
                                                                           //my mind a lot
    glBindVertexArray(VAO);
    glDrawElements( GL_TRIANGLE_STRIP , indices.size(), GL_UNSIGNED_INT,
                   0); // traiangle strip Works better ???? why ?? 
                       // GL_LINES_ADJACENCY also looks cool
                       // GL_LINES also looks cool
    glfwSwapBuffers(window);
    glfwPollEvents();
}
