#ifndef CONTROLS_H
#define CONTROLS_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Ball.h"
namespace Controls
{
   extern float radius;
   extern float yaw;
   extern float pitch;

   extern const float sensitivity;
   extern const float zoomSpeed;

   extern bool firstMouse;
   extern float lastX;
   extern float lastY;

   void handleInput(GLFWwindow *window, vector<Ball::Ball> &balls);
   void cursor_callback(GLFWwindow *window, double xpos, double ypos);
   void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
}

#endif // CONTROLS_H
