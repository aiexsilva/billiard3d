#version 410 core

// Vertex position, UV and normal
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

out vec2 TexCoord;
out vec3 Normal;

uniform mat4 MVP;

void main() {
    TexCoord = inUV;
    Normal   = inNormal;
    gl_Position = MVP * vec4(position, 1.0);
}