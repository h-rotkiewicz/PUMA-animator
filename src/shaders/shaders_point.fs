#version 330 core

in vec3 FragPos;
out vec4 FragColor;

uniform vec3 color;
uniform vec3 center;
uniform float maxDistance;

void main() {
    vec3 delta = FragPos - center;
    float distance = length(delta);
if(distance < maxDistance){
    FragColor = vec4(color, 1);
    }
    else{
    float newMaxDistance = 20* maxDistance;
    float alpha = 1.0 - clamp(distance / newMaxDistance, 0.0, 1.0);
    FragColor = vec4(0.0, 1.0, 0.0, alpha);
    }
}
