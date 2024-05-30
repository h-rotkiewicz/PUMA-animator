#pragma once
#include "camera.h"
#include "common.h"
#include "shader.h"
#include <unordered_map>


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, Camera &camera, auto &Parts);
GLFWwindow *init();
std::tuple<unsigned int, unsigned int, unsigned int>
bindBuffers(const std::vector<objl::Vertex> &vertices,
            const std::vector<unsigned int> &indices);
[[deprecated("has some bugs do not use")]] GLuint
bindTexture(std::string_view texturePath);
void preRender();

