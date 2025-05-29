// LoadShaders.h
#include <GL\gl.h>  

// ****************************************************************************
// GLuint LoadShaders(ShaderInfo*);
//
// Descrição:
// ----------
// Esta função recebe um array de estruturas do tipo ShaderInfo.
// Cada estrutura representa um shader a ser carregado e compilado.
//
// A estrutura ShaderInfo contém:
// - 'type': o tipo do shader, podendo ser, por exemplo:
//       GL_COMPUTE_SHADER
//       GL_VERTEX_SHADER
//       GL_TESS_CONTROL_SHADER
//       GL_TESS_EVALUATION_SHADER
//       GL_GEOMETRY_SHADER
//       GL_FRAGMENT_SHADER
//
// - 'filename': o nome do ficheiro onde está guardado o código fonte do shader.
//
// - 'shader': campo onde será armazenado o ID do objeto shader criado (após compilação).
//
// O array **deve terminar** com uma estrutura onde o campo 'type' tem o valor GL_NONE.
// Isso indica o fim do array.
//
// Exemplo de uso:
// ShaderInfo shaders[] = {
//     { GL_VERTEX_SHADER, "triangles.vert" },
//     { GL_FRAGMENT_SHADER, "triangles.frag" },
//     { GL_NONE, NULL }  // fim da lista
// };
//
// Retorno:
// --------
// - Se tudo correr bem, retorna o ID do programa OpenGL resultante da linkagem dos shaders.
// - Se ocorrer algum erro (na leitura, compilação ou linkagem), retorna 0.
//
// ****************************************************************************

// Define _DEBUG para ativar mensagens de erro (compilação/linkagem dos shaders)
#define _DEBUG

// Estrutura que representa a informação necessária para carregar um shader
typedef struct {
	GLenum       type;      // Tipo do shader (ex: GL_VERTEX_SHADER)
	const char* filename;  // Caminho do ficheiro com código fonte
	GLuint       shader;    // ID do shader compilado (preenchido pela função)
} ShaderInfo;

// Declaração da função que compila, linka e devolve um programa de shaders
GLuint LoadShaders(ShaderInfo*);
