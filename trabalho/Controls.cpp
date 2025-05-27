#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Ball.h"
#define height 800
#define width 1200

namespace Controls
{

   glm::mat4 globalRotationMatrix = glm::mat4(1.0f); 
   float globalScale = 1.0f;                         
   bool leftMousePressed = false;

   // Camera spherical coordinates for orbital view
   float radius = 3.0f; // distance from table center
   float yaw = 0.0f;    // horizontal angle
   float pitch = 20.0f; // vertical angle
   bool firstMouse = true;
   float lastX = width * 0.5f;
   float lastY = height * 0.5f;

   // Close window on ESC key press
   void handleInput(GLFWwindow *window, vector<Ball::Ball> &balls)
   {
      if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
         glfwSetWindowShouldClose(window, true);
      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      {
         glm::vec3 dir(0.0f, 0.0f, -1.0f);

         float speed = 100.0f;
         balls[15].setVelocity(dir * speed);
      }
   }

   void updateGlobalRotation(float deltaX, float deltaY)
   {
      float sensitivity = 0.005f;
      float angleX = deltaX * sensitivity;
      float angleY = deltaY * sensitivity;

      glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(0.0f, 1.0f, 0.0f));
      glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(1.0f, 0.0f, 0.0f));

      globalRotationMatrix = rotX * rotY * globalRotationMatrix;
   }

   // Update yaw/pitch from mouse movement (orbit control)
   void cursor_callback(GLFWwindow *window, double xpos, double ypos)
   {
      if (firstMouse)
      {
         lastX = float(xpos);
         lastY = float(ypos);
         firstMouse = false;
         return;
      }

      // adicionado ao precionar o botão esquerdo do rato calcula e atualiza a matriz global
      if (leftMousePressed)
      {
         float dx = float(xpos - lastX);
         float dy = float(ypos - lastY);

         updateGlobalRotation(dx, dy);
      }

      lastX = float(xpos);
      lastY = float(ypos);
   }

   // Zoom in/out by adjusting radius (scroll wheel)
   void scroll_callback(GLFWwindow *window, double /*xoffset*/, double yoffset)
   {
      float zoomSpeed = 0.1f;
      globalScale += yoffset * zoomSpeed; 

      if (globalScale < 0.1f)
         globalScale = 0.1f;
      if (globalScale > 10.0f)
         globalScale = 10.0f;
   }

   void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
   {
      if (button == GLFW_MOUSE_BUTTON_LEFT)
      {
         if (action == GLFW_PRESS)
            leftMousePressed = true;
         else if (action == GLFW_RELEASE)
            leftMousePressed = false;
      }
   }
}