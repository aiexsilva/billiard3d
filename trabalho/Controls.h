#ifndef CONTROLS_H
#define CONTROLS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Tamanhos da janela
#define height 800
#define width 1200

namespace Controls {
    // Variáveis de câmara (declaração apenas)
    extern float radius;
    extern float yaw;
    extern float pitch;
    extern const float sensitivity;
    extern const float zoomSpeed;
    extern bool firstMouse;
    extern float lastX;
    extern float lastY;

    // Funções de input
    void handleInput(GLFWwindow *window);
    void cursor_callback(GLFWwindow *window, double xpos, double ypos);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
}

#endif // CONTROLS_H
