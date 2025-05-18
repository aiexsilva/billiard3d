#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <glm/glm.hpp>

#define height 800
#define width 1200

namespace Controls{

    // Camera spherical coordinates for orbital view
    float radius = 3.0f; // distance from table center
    float yaw = 0.0f;    // horizontal angle
    float pitch = 20.0f; // vertical angle
    const float sensitivity = 0.1f;
    const float zoomSpeed = 1.0f;
    bool firstMouse = true;
    float lastX = width * 0.5f;
    float lastY = height * 0.5f;

    // Close window on ESC key press
    void handleInput(GLFWwindow *window){
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    }

    // Update yaw/pitch from mouse movement (orbit control)
    void cursor_callback(GLFWwindow *window, double xpos, double ypos){
        if (firstMouse){
            lastX = float(xpos);
            lastY = float(ypos);
            firstMouse = false;
        }
        float xoffset = float(xpos) - lastX;
        float yoffset = lastY - float(ypos);
        lastX = float(xpos);
        lastY = float(ypos);

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
    }

    // Zoom in/out by adjusting radius (scroll wheel)
    void scroll_callback(GLFWwindow *window, double /*xoffset*/, double yoffset){
        radius -= float(yoffset) * zoomSpeed;
        radius = glm::clamp(radius, 2.0f, 20.0f);
    }
}