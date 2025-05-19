#version 410 core

in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

// Material properties parsed from .mtl
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material uMaterial;
uniform sampler2D textureSampler;

void main() {
    // Sample the texture
    vec3 texColor = texture(textureSampler, TexCoord).rgb;

    // Ambient test lighting
    float ambientStrength = 0.3; // ambient light intensity
    vec3 ambient = ambientStrength * texColor;

    FragColor = vec4(ambient, 1.0);
}