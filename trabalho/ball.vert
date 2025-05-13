#version 410 core

// Vertex position and UV
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 inUV;

out vec2 TexCoord;
uniform mat4 MVP;

void main() {
    TexCoord = inUV;
    gl_Position = MVP * vec4(position, 1.0);
}