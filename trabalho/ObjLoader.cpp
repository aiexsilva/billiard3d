// ObjLoader.cpp
#include "ObjLoader.h"
#include <fstream>
#include <sstream>
#include <cstring>

namespace P3D
{

   bool LoadMTL(const char *path, std::unordered_map<std::string, Material> &materials)
   {
      std::ifstream file(path);
      if (!file.is_open())
      {
         std::cerr << "Could not open MTL file: " << path << "\n";
         return false;
      }
      std::string line;
      std::string currentName;
      while (std::getline(file, line))
      {
         std::istringstream iss(line);
         std::string prefix;
         iss >> prefix;
         if (prefix == "newmtl")
         {
            iss >> currentName;
            materials[currentName] = Material{};
         }
         else if (prefix == "Ka")
         {
            iss >> materials[currentName].Ka.x >> materials[currentName].Ka.y >> materials[currentName].Ka.z;
         }
         else if (prefix == "Kd")
         {
            iss >> materials[currentName].Kd.x >> materials[currentName].Kd.y >> materials[currentName].Kd.z;
         }
         else if (prefix == "Ks")
         {
            iss >> materials[currentName].Ks.x >> materials[currentName].Ks.y >> materials[currentName].Ks.z;
         }
         else if (prefix == "Ns")
         {
            iss >> materials[currentName].Ns;
         }
         else if (prefix == "map_Kd")
         {
            iss >> materials[currentName].map_Kd;
         }
         // ignore other prefixes
      }
      return true;
   }

   bool LoadObject(const char *path,
                   std::vector<glm::vec3> &out_vertices,
                   std::vector<glm::vec2> &out_uvs,
                   std::vector<glm::vec3> &out_normals,
                   Material &out_material)
   {
      // Temporary storage for vertex data
      std::vector<glm::vec3> temp_vertices;
      std::vector<glm::vec2> temp_uvs;
      std::vector<glm::vec3> temp_normals;

      // Map of materials parsed from .mtl
      std::unordered_map<std::string, Material> materials;
      std::string currentMaterial;

      // Determine base directory of OBJ file for resolving relative paths
      std::string objPathStr(path);
      std::string baseDir = objPathStr.substr(0, objPathStr.find_last_of("/\\") + 1);

      FILE *file = fopen(path, "r");
      if (!file)
      {
         std::cerr << "Unable to open OBJ file: " << path << "\n";
         return false;
      }

      while (true)
      {
         char lineHeader[128];
         int res = fscanf(file, "%s", lineHeader);
         if (res == EOF)
            break;

         if (strcmp(lineHeader, "mtllib") == 0)
         {
            char mtlName[128];
            fscanf(file, "%s\n", mtlName);
            // Build full path to the .mtl file and load it
            std::string fullMtlPath = baseDir + std::string(mtlName);
            LoadMTL(fullMtlPath.c_str(), materials);
         }
         else if (strcmp(lineHeader, "usemtl") == 0)
         {
            char matName[128];
            fscanf(file, "%s\n", matName);
            currentMaterial = matName;
            // assume single material per object, capture on first use
            out_material = materials[currentMaterial];
         }
         else if (strcmp(lineHeader, "v") == 0)
         {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
         }
         else if (strcmp(lineHeader, "vt") == 0)
         {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
         }
         else if (strcmp(lineHeader, "vn") == 0)
         {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
         }
         else if (strcmp(lineHeader, "f") == 0)
         {
            unsigned int vi[3], uvi[3], ni[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vi[0], &uvi[0], &ni[0],
                                 &vi[1], &uvi[1], &ni[1],
                                 &vi[2], &uvi[2], &ni[2]);
            if (matches != 9)
            {
               std::cerr << "Face parse error in: " << path << "\n";
               fclose(file);
               return false;
            }
            for (int i = 0; i < 3; ++i)
            {
               out_vertices.push_back(temp_vertices[vi[i] - 1]);
               out_uvs.push_back(temp_uvs[uvi[i] - 1]);
               out_normals.push_back(temp_normals[ni[i] - 1]);
            }
         }
         else
         {
            // skip unrecognized line
            char buffer[256];
            fgets(buffer, 256, file);
         }
      }

      fclose(file);
      return true;
   }

} // namespace P3D