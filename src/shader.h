#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


// From https://learnopengl.com/Getting-started/Shaders
class Shader

{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader(Shader const &other) = delete; //not sure if copying a shader is a good idea so delete for now
    Shader &operator=(Shader const &other) = delete;
    Shader(Shader &&other) {
        ID = other.ID;
        other.ID = 0;
    }
    Shader &operator=(Shader &&other) {
        ID = other.ID;
        other.ID = 0;
        return *this;
    }

    void use() const ;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
private:
    void checkCompileErrors(unsigned int shader, std::string type);
};
