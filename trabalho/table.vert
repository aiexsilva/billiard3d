#version 410 core

layout(location = 0) in vec3 position;
layout(location = 3) in vec3 aColor;

out vec3 FragPosition;
out vec3 Normal;
out vec3 vColor;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 View;

void main() {
    gl_Position   = MVP * vec4(position, 1.0);
    FragPosition  = vec3(model * vec4(position, 1.0));
    Normal = vec3(1.0,1.0,1.0);
    vColor       = aColor;
}
