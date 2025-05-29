#define _CRT_SECURE_NO_WARNINGS 

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "ObjLoader.h" 

using namespace std;

// Função para carregar um ficheiro .obj contendo geometria 3D
// Baseado no tutorial de: https://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
bool LoadObject(const std::string filepath,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals,
    std::string& out_mtlFilename)
{
    // Vetores temporários para armazenar dados do ficheiro .obj
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    // Abre o ficheiro para leitura
    FILE* filename = fopen(filepath.c_str(), "r");
    if (filename == NULL) {
        std::cout << "unable to open file\n";
        return -1;
    }

    // Leitura linha a linha
    while (true) {
        char lineHeader[128];
        int res = fscanf(filename, "%s", lineHeader); // Lê o tipo da linha (v, vt, vn, f, mtllib, etc.)
        if (res == EOF) break;

        else {
            // Se a linha indica o nome do ficheiro .mtl
            if (strcmp(lineHeader, "mtllib") == 0) {
                char MTLFilename[128];
                fscanf(filename, "%s", MTLFilename);
                out_mtlFilename = MTLFilename; // Armazena o nome do ficheiro de materiais
            }

            // Leitura de vértices
            if (strcmp(lineHeader, "v") == 0) {
                glm::vec3 vertex;
                fscanf(filename, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
                temp_vertices.push_back(vertex);
            }

            // Leitura de coordenadas de textura (UVs)
            else if (strcmp(lineHeader, "vt") == 0) {
                glm::vec2 uv;
                fscanf(filename, "%f %f\n", &uv.x, &uv.y);
                temp_uvs.push_back(uv);
            }

            // Leitura de normais
            else if (strcmp(lineHeader, "vn") == 0) {
                glm::vec3 normal;
                fscanf(filename, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
                temp_normals.push_back(normal);
            }

            // Leitura de faces (triângulos com vértice/uv/normal)
            else if (strcmp(lineHeader, "f") == 0) {
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                int matches = fscanf(filename, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                    &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                    &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                    &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

                if (matches != 9) {
                    std::cout << "Unable to read file\n";
                    return false;
                }

                // Constrói os triângulos a partir dos índices lidos
                for (int i = 0; i < 3; ++i) {
                    out_vertices.push_back(temp_vertices[vertexIndex[i] - 1]);
                    out_uvs.push_back(temp_uvs[uvIndex[i] - 1]);
                    out_normals.push_back(temp_normals[normalIndex[i] - 1]);
                }
            }
        }
    }

    fclose(filename); // Fecha o ficheiro
    return true;
}

// Função que carrega o ficheiro .mtl (Material Template Library)
bool LoadMTL(const std::string& filepath, std::vector<Material>& materials)
{
    Material currentMaterial;

    FILE* filename = fopen(filepath.c_str(), "r");
    if (filename == NULL) {
        std::cout << "unable to open MTL file\n";
        return false;
    }

    while (true) {
        char lineHeader[128];
        int res = fscanf(filename, "%s", lineHeader);
        if (res == EOF) break;

        else {
            // Nova definição de material
            if (strcmp(lineHeader, "newmtl") == 0) {
                // Salva o material anterior (caso exista)
                if (!currentMaterial.name.empty()) {
                    materials.push_back(currentMaterial);
                    currentMaterial = Material(); // Reinicializa para o novo material
                }

                char nameBuf[128];
                fscanf(filename, "%127s", nameBuf);
                currentMaterial.name = nameBuf;
            }

            // Propriedades do material
            if (strcmp(lineHeader, "Ns") == 0)
                fscanf(filename, "%f\n", &currentMaterial.Ns); // Brilho especular

            if (strcmp(lineHeader, "Ka") == 0) {
                glm::vec3 ambient;
                fscanf(filename, "%f %f %f\n", &ambient.x, &ambient.y, &ambient.z);
                currentMaterial.Ka = ambient; // Cor ambiente
            }

            if (strcmp(lineHeader, "Kd") == 0) {
                glm::vec3 diffuse;
                fscanf(filename, "%f %f %f\n", &diffuse.x, &diffuse.y, &diffuse.z);
                currentMaterial.Kd = diffuse; // Cor difusa
            }

            if (strcmp(lineHeader, "Ks") == 0) {
                glm::vec3 specular;
                fscanf(filename, "%f %f %f\n", &specular.x, &specular.y, &specular.z);
                currentMaterial.Ks = specular; // Cor especular
            }

            if (strcmp(lineHeader, "Ni") == 0)
                fscanf(filename, "%f\n", &currentMaterial.Ni); // Índice de refração

            if (strcmp(lineHeader, "d") == 0)
                fscanf(filename, "%f\n", &currentMaterial.d); // Transparência

            if (strcmp(lineHeader, "illum") == 0)
                fscanf(filename, "%d\n", &currentMaterial.illum); // Modelo de iluminação

            if (strcmp(lineHeader, "map_Kd") == 0) {
                char texturePath[128];
                fscanf(filename, "%s", texturePath);
                currentMaterial.map_Kd = texturePath; // Caminho para a textura difusa
            }
        }
    }

    fclose(filename);

    // Adiciona o último material lido (caso não tenha sido salvo ainda)
    if (!currentMaterial.name.empty()) {
        materials.push_back(currentMaterial);
    }

    return true;
}
