#pragma once
#include "camera.h"
#include "common.h"
#include "paths.h"
#include "shader.h"


// Vertex Shader Source
inline const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord; 
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Calculate the position of the vertex in world space
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Calculate the normal in world space and normalize it
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Pass texture coordinates to the fragment shader
    TexCoord = aTexCoord;
    
    // Calculate the final vertex position in clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

// Fragment Shader Source
inline const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color
}
)";

inline GLuint createShaderProgram() {
    GLint success;
    GLchar infoLog[512];

    // Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader Program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
// Function to render a point
// Helper struct, for doing reverse kinematics
struct Angle {
 private:
  float old_angle{};
  float new_angle{};

 public:
  auto get_old_angle() const { return old_angle; }
  auto get_new_angle() const { return new_angle; }

  auto get_angle_diff() const { return new_angle - old_angle; }

  auto add_angle(float angle) {
    old_angle = new_angle;
    new_angle += angle;
  }
};

struct ModelState {
  glm::vec3 axis{};
  glm::vec3 pivotPoint{};
  Angle     angle{};
  glm::mat4 model = glm::identity<glm::mat4>();
};

class ShaderManager {
 public:
  void updateShader(const Camera &camera, GLFWwindow *window);
  void RotatePart(RobotParts part, float angle);
  void addShader(RobotParts part, Shader &&shader);
  void render(std::unordered_map<RobotParts, Part> const &rendercontainer, GLFWwindow *window) ;

inline void renderPoint(const glm::vec3& point, const glm::vec3& normal, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, GLFWwindow* window) {
    // Compile and link shader program
    static GLuint shaderProgram = createShaderProgram();
    glUseProgram(shaderProgram);

    // Set the matrix uniforms
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

    // Setup the vertex data
    GLuint VAO, VBO;
    float vertices[] = { 
        point.x, point.y, point.z, // Position
        normal.x, normal.y, normal.z, // Normal
        0.0f, 0.0f // TexCoords (dummy since we're ignoring them)
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Dummy texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw the point
      glPointSize(20);
    glDrawArrays(GL_POINTS, 0, 1);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}


  std::unordered_map<RobotParts, Shader> const &getShaderMap() const;

 private:
  std::unordered_map<RobotParts, ModelState> StateMap{};
  std::unordered_map<RobotParts, Shader>     ShaderMap{};
};
