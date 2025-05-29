#define _CRT_SECURE_NO_WARNINGS  

#include <iostream>
#include <fstream>

#include <GL/glew.h>             
#include "ShaderLoader.h"        


// Fun��o para ler o conte�do de um ficheiro de shader e devolv�-lo como uma string terminada em '\0'
static const GLchar* ReadShader(const char* filename) {
    // Abre o ficheiro no final, em modo bin�rio (para obter o tamanho)
    std::ifstream ficheiro(filename, std::ifstream::ate | std::ifstream::binary);

    // Verifica se o ficheiro foi aberto corretamente
    if (ficheiro.is_open()) {
        // Obt�m o tamanho total do ficheiro
        std::streampos tamanhoDoFicheiroEmBytes = ficheiro.tellg();
        // Reposiciona o cursor para o in�cio do ficheiro
        ficheiro.seekg(0, std::ios::beg);

        // Aloca mem�ria para armazenar os dados do ficheiro (+1 para o terminador '\0')
        GLchar* source = new GLchar[int(tamanhoDoFicheiroEmBytes) + 1];
        ficheiro.read(source, tamanhoDoFicheiroEmBytes);
        source[tamanhoDoFicheiroEmBytes] = 0;  // Fecha a string com '\0'

        ficheiro.close();  // Fecha o ficheiro

        return const_cast<const GLchar*>(source);  // Retorna ponteiro para o conte�do
    }
    else {
        std::cerr << "Erro ao abrir o ficheiro '" << filename << "'" << std::endl;
    }

    return nullptr;
}


// Fun��o para carregar, compilar, e linkar um conjunto de shaders, retornando o ID do programa
GLuint LoadShaders(ShaderInfo* shaders) {
    if (shaders == nullptr) return 0;

    // Cria um programa de shaders vazio
    GLuint program = glCreateProgram();

    // Itera sobre todos os shaders fornecidos
    for (GLint i = 0; shaders[i].type != GL_NONE; i++) {
        shaders[i].shader = glCreateShader(shaders[i].type);  // Cria objeto shader

        const GLchar* source = ReadShader(shaders[i].filename);  // L� c�digo do shader
        if (source == NULL) {
            // Em caso de erro na leitura, limpa shaders criados at� o momento
            for (int j = 0; shaders[j].type != GL_NONE; j++) {
                if (shaders[j].shader != 0)
                    glDeleteShader(shaders[j].shader);
                shaders[j].shader = 0;
            }
            return 0;
        }

        glShaderSource(shaders[i].shader, 1, &source, NULL);  // Associa c�digo ao shader
        delete[] source;  // Liberta mem�ria alocada

        glCompileShader(shaders[i].shader);  // Compila o shader

        GLint compiled;
        glGetShaderiv(shaders[i].shader, GL_COMPILE_STATUS, &compiled);  // Verifica sucesso da compila��o
        if (!compiled) {
#ifdef _DEBUG
            GLsizei len;
            glGetShaderiv(shaders[i].shader, GL_INFO_LOG_LENGTH, &len);
            GLchar* log = new GLchar[len + 1];
            glGetShaderInfoLog(shaders[i].shader, len, &len, log);
            std::cerr << "Shader compilation failed: " << log << std::endl;
            delete[] log;
#endif
            // Em caso de erro na compila��o, limpa shaders
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
