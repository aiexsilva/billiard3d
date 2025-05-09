#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace std;

bool LoadObject(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals){

    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    FILE * filename = fopen(path, "r");
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