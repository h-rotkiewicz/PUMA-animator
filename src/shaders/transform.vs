#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;  // Not used, but kept for completeness
layout(location = 2) in vec2 aTexCoord; // Not used, but kept for completeness

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
