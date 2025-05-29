#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

// Evita ter que usar std:: o tempo todo
using namespace std;

// Estrutura que representa um material carregado do ficheiro .mtl
struct Material {
    std::string name;   // Nome do material
    glm::vec3 Ka;       // Cor ambiente (ambient color)
    glm::vec3 Kd;       // Cor difusa (diffuse color)
    glm::vec3 Ks;       // Cor especular (specular color)
    float Ns;           // Expoente do brilho especular (shininess)
    float Ni;           // Índice de refração (optical density)
    float d;            // Transparência (dissolve)
    int illum;          // Modelo de iluminação (illumination model)
    std::string map_Kd; // Caminho para textura difusa (diffuse texture map)
};

// Declaração da função para carregar um ficheiro .obj
// filepath: caminho do ficheiro .obj
// out_vertices: vetor para armazenar vértices
// out_uvs: vetor para armazenar coordenadas de textura (UVs)
// out_normals: vetor para armazenar normais
// out_mtlFilename: string para armazenar nome do ficheiro .mtl relacionado
bool LoadObject(const std::string filepath,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals,
    std::string& out_mtlFilename);

// Declaração da função para carregar um ficheiro .mtl
// filepath: caminho do ficheiro .mtl
// materials: vetor para armazenar os materiais carregados
bool LoadMTL(const std::string& filepath, std::vector<Material>& materials);

#endif
