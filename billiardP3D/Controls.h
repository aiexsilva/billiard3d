#ifndef CONTROLS_H
#define CONTROLS_H


#include <GLFW/glfw3.h>   
#include <glm/glm.hpp>    
#include "Ball.h"         

namespace Controls
{
    // Variáveis globais para controle da câmera orbital
    extern float radius;   // Distância da câmera ao centro da cena
    extern float yaw;      // Ângulo de rotação horizontal (em graus ou radianos)
    extern float pitch;    // Ângulo de rotação vertical

    // Constantes de controle de sensibilidade do mouse e zoom
    extern const float sensitivity;  // Sensibilidade ao mover o mouse
    extern const float zoomSpeed;   // Velocidade de zoom ao usar o scroll

    // Controle do estado do mouse
    extern bool firstMouse; // Verifica se é o primeiro movimento do mouse
    extern float lastX;     // Última posição X do cursor
    extern float lastY;     // Última posição Y do cursor

    // Variáveis de transformação global da cena
    extern glm::mat4 globalRotationMatrix; // Matriz acumulada de rotação
    extern float globalScale;              // Fator de escala da visualização
    extern bool leftMousePressed;          // Flag indicando se o botão esquerdo está pressionado

    // Funções de controle e input

    // Atualiza a matriz de rotação com base na movimentação do mouse
    void updateGlobalRotation(float deltaX, float deltaY);

    // Trata entradas de teclado (como ESC e ESPAÇO)
    void handleInput(GLFWwindow* window, vector<Ball::Ball>& balls);

    // Callback de movimento do cursor (GLFW chama automaticamente)
    void cursor_callback(GLFWwindow* window, double xpos, double ypos);

    // Callback do scroll do mouse para zoom
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    // Callback para cliques de botão do mouse
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
}

#endif // CONTROLS_H
