#pragma once
#include "main.h"

struct Settings {
  // settings
  constexpr static unsigned int SCR_WIDTH = 800;
  constexpr static unsigned int SCR_HEIGHT = 600;
};


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, float &angle, glm::vec3 &RotationAxis);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
GLFWwindow *init();
std::tuple<unsigned int, unsigned int, unsigned int>
bindBuffers(const std::vector<objl::Vertex> &vertices,
            const std::vector<unsigned int> &indices);
GLuint bindTexture();
void preRender( GLFWwindow *window, const GLuint & texture1, ModelParams &modelParams);
void updateShader(Shader &Shader, const ModelParams &modelParams);

void render(const GLuint &VAO, const auto &indices, GLFWwindow * window) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT,
                   0); // traiangle strip Works better ???? why ??
    glfwSwapBuffers(window);
    glfwPollEvents();
}
