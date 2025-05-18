#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "ObjLoader.h"

using namespace std;


bool LoadObject(const std::string filepath, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals, std::string& out_mtlFilename){

    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    FILE * filename = fopen(filepath.c_str(), "r");
    if(filename == NULL){
        std::cout<<"unable to open file\n";
        return -1;
    }

    while(true){
        char lineHeader[128];
        int res = fscanf(filename, "%s", lineHeader);
        if (res == EOF){
            break;
        }else{
            //gets the name of the mtl file off obj file
            if(strcmp(lineHeader, "mtllib") == 0){
                char MTLFilename[128];
                fscanf(filename, "%s", MTLFilename);
                out_mtlFilename = MTLFilename;
            }
            //read vertices
            if (strcmp(lineHeader, "v") == 0 ){
                glm::vec3 vertex;
                fscanf(filename, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
                temp_vertices.push_back(vertex);
            //read uvs
            }else if (strcmp(lineHeader, "vt") == 0 ){
                glm::vec2 uv;
                fscanf(filename, "%f %f\n", &uv.x, &uv.y );
                temp_uvs.push_back(uv);
            //read normals
            }else if (strcmp(lineHeader, "vn") == 0 ){
                glm::vec3 normal;
                fscanf(filename, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
                temp_normals.push_back(normal);
            }else if ( strcmp( lineHeader, "f" ) == 0 ){
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                int matches = fscanf(filename, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                         &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                         &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                         &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

                if (matches != 9){
                    std::cout << "Unable to read file\n";
                    return false;
                }

                for (int i = 0; i < 3; ++i) {
                    out_vertices.push_back(temp_vertices[vertexIndex[i] - 1]);
                    out_uvs.push_back(temp_uvs[uvIndex[i] - 1]);
                    out_normals.push_back(temp_normals[normalIndex[i] - 1]);
                }   
            }
        }
    }
    
    fclose(filename); 
    return true;
}

bool LoadMTL(const std::string& filepath, std::vector<Material>& materials) {
    Material currentMaterial;
    FILE * filename = fopen(filepath.c_str(), "r");
    if(filename == NULL){
        std::cout << "unable to open MTL file\n";
        return false;
    }

    while(true) {
        char lineHeader[128];
        int res = fscanf(filename, "%s", lineHeader);
        if (res == EOF) {
            break;
        } else {
            if (strcmp(lineHeader, "newmtl") == 0)
            {
                // salva o material anterior (se existir)
                if (!currentMaterial.name.empty())
                {
                    materials.push_back(currentMaterial);
                    currentMaterial = Material(); // reinicia todos os campos
                }
                char nameBuf[128];
                fscanf(filename, "%127s", nameBuf);
                currentMaterial.name = nameBuf;
            }
            if (strcmp(lineHeader, "Ns") == 0)
            {
                fscanf(filename, "%f\n", &currentMaterial.Ns);
            }
            if (strcmp(lineHeader, "Ka") == 0)
            {
                glm::vec3 ambient;
                fscanf(filename, "%f %f %f\n", &ambient.x, &ambient.y, &ambient.z);
                currentMaterial.Ka = ambient;
            }
            if (strcmp(lineHeader, "Kd") == 0)
            {
                glm::vec3 diffuse;
                fscanf(filename, "%f %f %f\n", &diffuse.x, &diffuse.y, &diffuse.z);
                currentMaterial.Kd = diffuse;
            }
            if (strcmp(lineHeader, "Ks") == 0)
            {
                glm::vec3 specular;
                fscanf(filename, "%f %f %f\n", &specular.x, &specular.y, &specular.z);
                currentMaterial.Ks = specular;
            }
            if (strcmp(lineHeader, "Ni") == 0)
            {
                fscanf(filename, "%f\n", &currentMaterial.Ni);
            }
            if (strcmp(lineHeader, "d") == 0)
            {
                fscanf(filename, "%f\n", &currentMaterial.d);
            }
            if (strcmp(lineHeader, "illum") == 0)
            {
                fscanf(filename, "%d\n", &currentMaterial.illum);
            }
            if (strcmp(lineHeader, "map_Kd") == 0)
            {
                char texturePath[128];
                fscanf(filename, "%s", texturePath);
                currentMaterial.map_Kd = texturePath;
            }
            
        }
    }
    fclose(filename);

    if (!currentMaterial.name.empty()) {
        materials.push_back(currentMaterial);
    }

    return true;
}
