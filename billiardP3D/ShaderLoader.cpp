#define _CRT_SECURE_NO_WARNINGS  

#include <iostream>
#include <fstream>

#include <GL/glew.h>             
#include "ShaderLoader.h"        


// Função para ler o conteúdo de um ficheiro de shader e devolvê-lo como uma string terminada em '\0'
static const GLchar* ReadShader(const char* filename) {
    // Abre o ficheiro no final, em modo binário (para obter o tamanho)
    std::ifstream ficheiro(filename, std::ifstream::ate | std::ifstream::binary);

    // Verifica se o ficheiro foi aberto corretamente
    if (ficheiro.is_open()) {
        // Obtém o tamanho total do ficheiro
        std::streampos tamanhoDoFicheiroEmBytes = ficheiro.tellg();
        // Reposiciona o cursor para o início do ficheiro
        ficheiro.seekg(0, std::ios::beg);

        // Aloca memória para armazenar os dados do ficheiro (+1 para o terminador '\0')
        GLchar* source = new GLchar[int(tamanhoDoFicheiroEmBytes) + 1];
        ficheiro.read(source, tamanhoDoFicheiroEmBytes);
        source[tamanhoDoFicheiroEmBytes] = 0;  // Fecha a string com '\0'

        ficheiro.close();  // Fecha o ficheiro

        return const_cast<const GLchar*>(source);  // Retorna ponteiro para o conteúdo
    }
    else {
        std::cerr << "Erro ao abrir o ficheiro '" << filename << "'" << std::endl;
    }

    return nullptr;
}


// Função para carregar, compilar, e linkar um conjunto de shaders, retornando o ID do programa
GLuint LoadShaders(ShaderInfo* shaders) {
    if (shaders == nullptr) return 0;

    // Cria um programa de shaders vazio
    GLuint program = glCreateProgram();

    // Itera sobre todos os shaders fornecidos
    for (GLint i = 0; shaders[i].type != GL_NONE; i++) {
        shaders[i].shader = glCreateShader(shaders[i].type);  // Cria objeto shader

        const GLchar* source = ReadShader(shaders[i].filename);  // Lê código do shader
        if (source == NULL) {
            // Em caso de erro na leitura, limpa shaders criados até o momento
            for (int j = 0; shaders[j].type != GL_NONE; j++) {
                if (shaders[j].shader != 0)
                    glDeleteShader(shaders[j].shader);
                shaders[j].shader = 0;
            }
            return 0;
        }

        glShaderSource(shaders[i].shader, 1, &source, NULL);  // Associa código ao shader
        delete[] source;  // Liberta memória alocada

        glCompileShader(shaders[i].shader);  // Compila o shader

        GLint compiled;
        glGetShaderiv(shaders[i].shader, GL_COMPILE_STATUS, &compiled);  // Verifica sucesso da compilação
        if (!compiled) {
#ifdef _DEBUG
            GLsizei len;
            glGetShaderiv(shaders[i].shader, GL_INFO_LOG_LENGTH, &len);
            GLchar* log = new GLchar[len + 1];
            glGetShaderInfoLog(shaders[i].shader, len, &len, log);
            std::cerr << "Shader compilation failed: " << log << std::endl;
            delete[] log;
#endif
            // Em caso de erro na compilação, limpa shaders
            for (int j = 0; shaders[j].type != GL_NONE; j++) {
                if (shaders[j].shader != 0)
                    glDeleteShader(shaders[j].shader);
                shaders[j].shader = 0;
            }
            return 0;
        }

        glAttachShader(program, shaders[i].shader);  // Anexa shader compilado ao programa
    }

    glLinkProgram(program);  // Linka todos os shaders no programa final

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);  // Verifica se linkagem teve sucesso
    if (!linked) {
#ifdef _DEBUG
        GLsizei len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        GLchar* log = new GLchar[len + 1];
        glGetProgramInfoLog(program, len, &len, log);
        std::cerr << "Shader linking failed: " << log << std::endl;
        delete[] log;
#endif
        // Em caso de erro na linkagem, limpa os shaders
        for (int j = 0; shaders[j].type != GL_NONE; j++) {
            if (shaders[j].shader != 0)
                glDeleteShader(shaders[j].shader);
            shaders[j].shader = 0;
        }

        return 0;
    }

    return program;  // Retorna ID do programa OpenGL com shaders prontos para uso
}
