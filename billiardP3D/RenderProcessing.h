#ifndef RENDERPROCESSING_H
#define RENDERPROCESSING_H

// Inclus�es de bibliotecas padr�o e externas necess�rias
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ObjLoader.h"

// Defini��o do namespace para evitar conflitos de nomes
namespace RenderProcessing
{
    // Classe principal respons�vel pelo gerenciamento de renderiza��o
    class RenderPro
    {
    public:
        // Carrega manualmente um vetor de v�rtices (sem carregar de um arquivo .obj)
        void ManualLoad(const std::vector<glm::vec3>& Vertices);

        // Carrega modelo 3D a partir de um arquivo .obj
        bool Load(const std::string filepath);

        // Inicializa e instala buffers (VBOs, VAOs etc.) necess�rios para renderizar
        void Install();

        // Define o shader que ser� usado na renderiza��o
        void Set(GLuint shader);

        // Renderiza o modelo na tela principal
        void Render(glm::vec3 position, glm::vec3 rotation);

        // Renderiza o modelo em uma minimapa com proje��o ortogr�fica
        void RenderInMinimap(glm::vec3 position, glm::vec3 rotation);

        // Define a escala do modelo
        void SetScale(glm::vec3 newScale);

        // Define as cores da mesa (pode ser usado como cores dos v�rtices)
        void SetTableColors(vector<glm::vec3> tableColors);

        // Processa entrada de teclado do usu�rio (definido externamente)
        void processInput(GLFWwindow* window);

        // Define a janela GLFW para capturar inputs e renderizar
        void SetWindow(GLFWwindow* window);

        // Carrega uma textura a partir de um caminho de arquivo
        GLuint LoadTexture(const std::string filepath);

        // Rota��o aplicada ao modelo (usada para animar ou posicionar)
        glm::vec3 modelRotation = glm::vec3(0.0f);

        // Posi��o da c�mera usada na renderiza��o
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);

    private:
        // Lista de v�rtices do modelo 3D
        std::vector<glm::vec3> vertices;

        // Coordenadas de textura (UVs) do modelo
        std::vector<glm::vec2> uvs;

        // Normais dos v�rtices, usadas em c�lculos de ilumina��o
        std::vector<glm::vec3> normals;

        // Escala atual do modelo
        glm::vec3 scale = glm::vec3(1.0f);

        // Cores definidas para os v�rtices da mesa
        vector<glm::vec3> tableColors;

        // Material atual do objeto (cont�m propriedades como Ka, Kd, Ks etc.)
        Material currentMaterial;

        // ID do shader associado � renderiza��o
        GLuint shader;

        // Localiza��o do uniform da textura no shader
        GLuint textureLoc;

        // ID da textura associada ao modelo
        GLuint texture = 0;

        // Identificadores para objetos de buffer OpenGL
        GLuint VAO, VBO, colorVBO;

        // Localiza��es dos uniforms de matriz no shader
        GLuint mvpLoc;
        GLuint modelLoc;
        GLuint viewLoc;
        GLuint modelViewLoc;

        // Ponteiro para a janela GLFW, usado para processar entrada do usu�rio
        GLFWwindow* window = nullptr;
    };
}

#endif // RENDERPROCESSING_H
