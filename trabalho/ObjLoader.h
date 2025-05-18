#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

using namespace std;

struct Material {
    std::string name;
    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;
    float Ns;
    float Ni;
    float d;
    int illum;
    std::string map_Kd;
};

bool LoadObject(const std::string filepath, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals, std::string& out_mtlFilename);
bool LoadMTL(const std::string& filepath, std::vector<Material>& materials);

#endif
