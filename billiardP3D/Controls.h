#ifndef CONTROLS_H
#define CONTROLS_H


#include <GLFW/glfw3.h>   
#include <glm/glm.hpp>    
#include "Ball.h"         

namespace Controls
{
    // Vari�veis globais para controle da c�mera orbital
    extern float radius;   // Dist�ncia da c�mera ao centro da cena
    extern float yaw;      // �ngulo de rota��o horizontal (em graus ou radianos)
    extern float pitch;    // �ngulo de rota��o vertical

    // Constantes de controle de sensibilidade do mouse e zoom
    extern const float sensitivity;  // Sensibilidade ao mover o mouse
    extern const float zoomSpeed;   // Velocidade de zoom ao usar o scroll

    // Controle do estado do mouse
    extern bool firstMouse; // Verifica se � o primeiro movimento do mouse
    extern float lastX;     // �ltima posi��o X do cursor
    extern float lastY;     // �ltima posi��o Y do cursor

    // Vari�veis de transforma��o global da cena
    extern glm::mat4 globalRotationMatrix; // Matriz acumulada de rota��o
    extern float globalScale;              // Fator de escala da visualiza��o
    extern bool leftMousePressed;          // Flag indicando se o bot�o esquerdo est� pressionado

    // Fun��es de controle e input

    // Atualiza a matriz de rota��o com base na movimenta��o do mouse
    void updateGlobalRotation(float deltaX, float deltaY);

    // Trata entradas de teclado (como ESC e ESPA�O)
    void handleInput(GLFWwindow* window, vector<Ball::Ball>& balls);

    // Callback de movimento do cursor (GLFW chama automaticamente)
    void cursor_callback(GLFWwindow* window, double xpos, double ypos);

    // Callback do scroll do mouse para zoom
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    // Callback para cliques de bot�o do mouse
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
}

#endif // CONTROLS_H
