#define _CRT_SECURE_NO_WARNINGS  


#include <iostream>
#include <GL/glew.h>             
#include <GLFW/glfw3.h>          
#include <GL/gl.h>               
#include <glm/glm.hpp>           
#include <glm/gtc/matrix_transform.hpp> 
#include <vector>                
#include "Ball.h"                

#define height 800
#define width 1200

namespace Controls  
{
    // Matriz de rota��o global (acumulada ao mover o mouse)
    glm::mat4 globalRotationMatrix = glm::mat4(1.0f);

    // Fator de escala global (usado para zoom)
    float globalScale = 1.0f;

    // Controle do clique do bot�o esquerdo do mouse
    bool leftMousePressed = false;

    // ======= Controle da C�mera Orbital =======
    float radius = 3.0f;  // Dist�ncia da c�mera at� o centro da cena
    float yaw = 0.0f;     // �ngulo horizontal (em graus ou rad)
    float pitch = 20.0f;  // �ngulo vertical
    bool firstMouse = true;          // Primeiro movimento do mouse?
    float lastX = width * 0.5f;      // Posi��o X anterior do mouse
    float lastY = height * 0.5f;     // Posi��o Y anterior do mouse

    // Fun��o de input para teclado (chamada por frame)
    void handleInput(GLFWwindow* window, vector<Ball::Ball>& balls)
    {
        // Fecha a janela se ESC for pressionado
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

		// Ao pressionar espa�o, aplica velocidade � bola branca (�ndice 15)
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            glm::vec3 dir(0.0f, 0.0f, -1.0f); // Dire��o do movimento
            float speed = 100.0f;
            balls[15].setVelocity(dir * speed);  // Aplica velocidade � bola
        }
    }

    // Atualiza a matriz de rota��o com base no movimento do mouse
    void updateGlobalRotation(float deltaX, float deltaY)
    {
        float sensitivity = 0.005f;  // Sensibilidade do mouse

        float angleX = deltaX * sensitivity;  // Rota��o no eixo Y (horizontal)
        float angleY = deltaY * sensitivity;  // Rota��o no eixo X (vertical)

        // Cria matrizes de rota��o
        glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(0.0f, 1.0f, 0.0f));  // Rota��o em Y
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(1.0f, 0.0f, 0.0f));  // Rota��o em X

        // Aplica rota��o acumulada
        globalRotationMatrix = rotX * rotY * globalRotationMatrix;
    }

    // Callback do movimento do cursor (chamada autom�tica pelo GLFW)
    void cursor_callback(GLFWwindow* window, double xpos, double ypos) // posicao do rato
    {
        if (firstMouse)  // Captura inicial do mouse para evitar "salto"
        {
            lastX = float(xpos);
            lastY = float(ypos);
            firstMouse = false;
            return;
        }

        // Apenas se o bot�o esquerdo estiver pressionado
        if (leftMousePressed)
        {
            float dx = float(xpos - lastX);  // Diferen�a horizontal
            float dy = float(ypos - lastY);  // Diferen�a vertical

            updateGlobalRotation(dx, dy);    // Aplica a rota��o com base no movimento
        }

        // Atualiza as posi��es do mouse
        lastX = float(xpos);
        lastY = float(ypos);
    }

    // Callback do scroll do mouse (zoom)
    void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset)
    {
        float zoomSpeed = 0.1f;
        globalScale += yoffset * zoomSpeed;  // Zoom in/out com scroll

        // Limita o zoom m�nimo e m�ximo
        if (globalScale < 0.1f)
            globalScale = 0.1f;
        if (globalScale > 10.0f)
            globalScale = 10.0f;
    }

    // Callback de clique do mouse (pressionar ou soltar bot�o)
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) // clicar na tecla do rato
    {
        // Se bot�o esquerdo do mouse for pressionado ou solto
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
                leftMousePressed = true;
            else if (action == GLFW_RELEASE)
                leftMousePressed = false;
        }
    }
}
