// ObjLoader.h
#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace P3D
{
   // Material properties read from .mtl
   struct Material
   {
      glm::vec3 Ka;       // ambient reflection coefficient
      glm::vec3 Kd;       // diffuse reflection coefficient
      glm::vec3 Ks;       // specular reflection coefficient
      float Ns;           // shininess exponent
      std::string map_Kd; // diffuse texture filename
   };

   // Parse a .mtl file and fill the materials map
   bool LoadMTL(const char *path, std::unordered_map<std::string, Material> &materials);

   // Load an OBJ, including vertices, uvs, normals and the associated material
   bool LoadObject(const char *path,
                   std::vector<glm::vec3> &out_vertices,
                   std::vector<glm::vec2> &out_uvs,
                   std::vector<glm::vec3> &out_normals,
                   Material &out_material);
}

#endif // OBJ_LOADER_H