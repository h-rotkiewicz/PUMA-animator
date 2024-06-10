/* Simple Phong lighting model*/
#version 330 core

in vec2 TexCoord; // from vertex shader
in vec3 FragPos;  // from vertex shader
in vec3 Normal;   // from vertex shader

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 lightPos;  // The position of the light source
uniform vec3 viewPos;   // The position of the camera
uniform vec3 lightColor;

void main() {
    // Sample the texture
    vec4 texColor = texture(texture1, TexCoord);

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 lighting = (ambient + diffuse + specular) ;

    FragColor = vec4(lighting, 1.0) * texColor;
    
}
