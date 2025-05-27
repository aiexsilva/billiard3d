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

   // adicionado variaveis glovais para rotação e escala
   extern glm::mat4 globalRotationMatrix;
   extern float globalScale;
   extern bool leftMousePressed;

   // adicionado função para atualizar a rotação
   void updateGlobalRotation(float deltaX, float deltaY);

   void handleInput(GLFWwindow *window, vector<Ball::Ball> &balls);
   void cursor_callback(GLFWwindow *window, double xpos, double ypos);
   void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
   void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
}

#endif // CONTROLS_H
