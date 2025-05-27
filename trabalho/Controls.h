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

<<<<<<< Updated upstream
    // Funções de input
    void handleInput(GLFWwindow *window);
    void cursor_callback(GLFWwindow *window, double xpos, double ypos);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
=======
   extern bool firstMouse;
   extern float lastX;
   extern float lastY;

   //adicionado variaveis glovais para rotação e escala
   extern glm::mat4 globalRotationMatrix;
   extern float globalScale;
   extern bool leftMousePressed;

   void handleInput(GLFWwindow *window);
   void cursor_callback(GLFWwindow *window, double xpos, double ypos);
   void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
   void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

   //adicionado função para atualizar a rotação
   void updateGlobalRotation(float deltaX, float deltaY);
>>>>>>> Stashed changes
}

#endif // CONTROLS_H
