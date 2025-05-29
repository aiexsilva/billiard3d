#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include "ShaderLoader.h"
#include "ObjLoader.h"
#include "RenderProcessing.h"
#include "Controls.h"
#include "Ball.h"

// Protótipos
void print_gl_info();
void print_error(int error, const char* description);
void init(void);
void display(void);

// Dimensões da janela
#define height 800
#define width 1200

// Instância do renderizador da mesa
RenderProcessing::RenderPro tableRender;
// Vetor com 16 bolas (a mesa de sinuca)
vector<Ball::Ball> balls(16);

// Tamanho do minimapa em pixels
const int minimapSize = 400;

// Cantos da mesa (paralelogramo)
glm::vec3 tableRUcorner(0.5f, 0.15f, 1.0f);
glm::vec3 tableRDcorner(0.5f, -0.15f, 1.0f);
glm::vec3 tableLUcorner(-0.5f, 0.15f, 1.0f);
glm::vec3 tableLDcorner(-0.5f, -0.15f, 1.0f);

int main()
{
    // Inicialização do GLFW
    glfwInit();
    /*
    // Configurações específicas para MacOS (comentadas)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    */

    // Cria janela GLFW
    GLFWwindow* window = glfwCreateWindow(width, height, "Billiards", NULL, NULL);
    if (!window)
    {
        cout << "failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Registra callbacks para controles do rato
    glfwSetCursorPosCallback(window, Controls::cursor_callback);
    glfwSetScrollCallback(window, Controls::scroll_callback);
    glfwSetMouseButtonCallback(window, Controls::mouse_button_callback);

    // Esconde o cursor e prende-o na janela para controlo tipo FPS
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Inicializa GLEW (carrega funções OpenGL modernas)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cout << "failed to initialize glew\n";
        return -1;
    }

    // Ativa teste de profundidade (z-buffer)
    glEnable(GL_DEPTH_TEST);

    // Cor de fundo preta
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Define os vértices do paralelepípedo que será a mesa (8 vértices)
    GLfloat paraleli_vertices[] = {
        // front face
        -0.5f, -0.15f, 1.0f,  // front bottom left
         0.5f, -0.15f, 1.0f,  // front bottom right
         0.5f,  0.15f, 1.0f,  // front top right
        -0.5f,  0.15f, 1.0f,  // front top left

        // back face
        -0.5f, -0.15f, -1.0f, // back bottom left
         0.5f, -0.15f, -1.0f, // back bottom right
         0.5f,  0.15f, -1.0f, // back top right
        -0.5f,  0.15f, -1.0f  // back top left
    };

    // Índices para formar triângulos (6 faces x 2 triângulos cada)
    GLuint paraleli_indices[] = {
        0, 1, 2, 0, 2, 3,  // front face
        4, 5, 6, 4, 6, 7,  // back face
        0, 3, 7, 0, 7, 4,  // left face
        1, 2, 6, 1, 6, 5,  // right face
        2, 3, 7, 2, 7, 6,  // top face
        0, 1, 5, 0, 5, 4   // bottom face
    };

    // Constrói vetor de vértices para a mesa (usando índices para expandir)
    vector<glm::vec3> tableVerts;
    for (int i = 0; i < 36; ++i)
    {
        int idx = paraleli_indices[i];
        glm::vec3 v(
            paraleli_vertices[idx * 3 + 0],
            paraleli_vertices[idx * 3 + 1],
            paraleli_vertices[idx * 3 + 2]);
        tableVerts.push_back(v);
    }

    // Define shaders para mesa e bolas
    ShaderInfo table_shader[] = {
        {GL_VERTEX_SHADER, "table.vert"},
        {GL_FRAGMENT_SHADER, "table.frag"},
        {GL_NONE, NULL}
    };
    ShaderInfo ball_shader[] = {
        {GL_VERTEX_SHADER, "ball.vert"},
        {GL_FRAGMENT_SHADER, "ball.frag"},
        {GL_NONE, NULL}
    };

    // Carrega e compila os shaders da mesa
    GLuint tableShader = LoadShaders(table_shader);
    if (!tableShader)
        exit(EXIT_FAILURE);
    // Carrega e compila os shaders das bolas
    GLuint ballShader = LoadShaders(ball_shader);
    if (!ballShader)
        exit(EXIT_FAILURE);

    // Carrega os vértices da mesa no objeto de renderização
    tableRender.ManualLoad(tableVerts);

    // Define cores para as faces da mesa (tons de verde)
    vector<glm::vec3> tableColors;
    tableColors.reserve(tableVerts.size());
    glm::vec3 faceGreen[6] = {
        {0.2f, 0.8f, 0.2f},
        {0.1f, 0.6f, 0.1f},
        {0.15f, 0.75f, 0.15f},
        {0.25f, 0.85f, 0.25f},
        {0.3f, 0.9f, 0.3f},
        {0.05f, 0.5f, 0.05f}
    };
    for (int face = 0; face < 6; ++face)
        for (int i = 0; i < 6; ++i)
            tableColors.push_back(faceGreen[face]);

    tableRender.SetTableColors(tableColors);

    // Inicializa buffers, VBO, VAO etc da mesa
    tableRender.Install();

    // Define shader da mesa para renderização
    tableRender.Set(tableShader);

    // Inicializa objetos das bolas e carrega seus modelos
    for (int i = 0; i < balls.size(); i++)
    {
        RenderProcessing::RenderPro ballRenderPro;

        // Associa renderizador ao objeto bola
        balls[i].getRenderPro(ballRenderPro);

        // Define shader para bola
        ballRenderPro.Set(ballShader);

        // Carrega modelo OBJ da bola correspondente (nome baseado no índice)
        ballRenderPro.Load("PoolBalls/Ball" + std::to_string(i + 1) + ".obj");

        // Inicializa buffers e VAO da bola
        ballRenderPro.Install();

        // Ajusta escala para bola pequena
        ballRenderPro.SetScale(glm::vec3(0.05f));

        // Atualiza referência ao renderizador na bola
        balls[i].setRenderPro(ballRenderPro);
    }

    // Espaçamento entre bolas na formação
    float r = balls[0].getRadius();
    float spacing = r * 3.0f * 1.1f;
    float tableY = 1.0f;

    // Posição central para rack de bolas
    glm::vec3 rackCenter = glm::vec3(0.0f, tableY, 1.2f);
    int idx = 0;
    // Monta rack em formato triangular (5 linhas, 1 a 5 bolas)
    for (int row = 0; row < 5; ++row)
    {
        int ballsInRow = row + 1;
        float startX = rackCenter.x - (ballsInRow - 1) * spacing * 0.5f;
        float z = rackCenter.z - row * spacing;
        for (int j = 0; j < ballsInRow; ++j)
        {
            float x = startX + j * spacing;
            balls[idx].setPosition({ x, tableY, z });
            balls[idx].setOrientation({ 0.0f, 1.0f, 0.0f });
            ++idx;
        }
    }

    // Bola branca posicionada separadamente no início da mesa
    balls[15].setPosition({ 0.0f, tableY, 22.5f });
    balls[15].setOrientation({ 0, 1, 0 });

    double lastTime = glfwGetTime();

    // Loop principal
    while (!glfwWindowShouldClose(window))
    {
        // Processa entrada do usuário (mouse, teclado)
        Controls::handleInput(window, balls);

        // Desativa teste de scissor para limpar tela toda
        glDisable(GL_SCISSOR_TEST);

        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        glViewport(0, 0, fbW, fbH);

        // Limpa cor e profundidade da tela
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Renderiza a mesa na posição e orientação especificadas
        glm::vec3 tablePosition(0.0f, -0.12f, 0.0f);
        glm::vec3 tableOrientation(0.0f, 0.0f, 0.0f);
        tableRender.SetScale(glm::vec3(1.5f));
        tableRender.Render(tablePosition, tableOrientation);

        // Calcula delta time para atualizar física
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;
        deltaTime = std::min(deltaTime, 1.0f / 60.0f);

        // Atualiza cada bola: movimento e renderização
        for (auto& ball : balls)
        {
            ball.update(deltaTime, glm::vec2(-14.0f, -29.0f), glm::vec2(14.0f, 29.0f));

            RenderProcessing::RenderPro ballMainRender;
            ball.getRenderPro(ballMainRender);
            ballMainRender.modelRotation = tableRender.modelRotation;
            ballMainRender.SetWindow(window);

            ballMainRender.Render(ball.getPosition(), ball.getOrientation());
        }

        // Verifica e resolve colisões entre todas as bolas
        for (size_t i = 0; i < balls.size(); ++i)
        {
            for (size_t j = i + 1; j < balls.size(); ++j)
            {
                balls[i].handleBallCollision(balls[j]);
            }
        }

        // Configuração e desenho do minimapa (pequena visão da mesa)
        int minix = fbW - minimapSize - 10;
        int miniy = fbH - minimapSize - 10;
        const int border = 1;

        // Limpa borda branca ao redor do minimapa
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_SCISSOR_TEST);
        glScissor(
            minix - border,
            miniy - border,
            minimapSize + 2 * border,
            minimapSize + 2 * border);
        glClear(GL_COLOR_BUFFER_BIT);

        // Limpa área preta dentro da borda
        glScissor(minix, miniy, minimapSize, minimapSize);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        // Ajusta viewport para minimapa
        glViewport(minix, miniy, minimapSize, minimapSize);

        // Renderiza mesa e bolas no minimapa
        tableRender.RenderInMinimap(tablePosition, tableOrientation);
        tableRender.SetScale(glm::vec3(1.5f));
        for (auto& ball : balls)
        {
            RenderProcessing::RenderPro ballMainRender;
            ball.getRenderPro(ballMainRender);
            ballMainRender.RenderInMinimap(ball.getPosition(), ball.getOrientation());
        }

        // Troca buffers para mostrar frame e processa eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Finaliza GLFW e libera recursos
    glfwTerminate();
    return 0;
}


/*
 no vert tem funcoes para definir a posicao do modelo, camera, etc.

 no frag tem funcoes para definir a cor do material, luzes, etc.
 consuante os valor das luzes, ambiente, direcional, pontual e spot, ele adiciona a cor final as luzes
*/