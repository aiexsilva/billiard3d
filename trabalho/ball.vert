#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPosition;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 View;

void main() {
    gl_Position   = MVP * vec4(position, 1.0);
    FragPosition  = vec3(model * vec4(position, 1.0));
    Normal        = mat3(transpose(inverse(model))) * inNormal;
    TexCoord      = inUV;
}