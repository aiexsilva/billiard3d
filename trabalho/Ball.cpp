// Ball.cpp
#include "Ball.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp> // for glm::angleAxis
#include <glm/gtc/constants.hpp>  // for glm::pi
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace P3D
{

   // Static member initialization
   bool Ball::meshInitialized = false;
   GLuint Ball::VAO = 0;
   GLuint Ball::VBO = 0;
   std::vector<glm::vec3> Ball::vertices;
   std::vector<glm::vec2> Ball::uvs;
   std::vector<glm::vec3> Ball::normals;

   Ball::Ball(const std::string &baseName)
       : objPath("PoolBalls/" + baseName + ".obj"),
         mtlPath("PoolBalls/" + baseName + ".mtl"),
         texPath("PoolBalls/PoolBalluv" + baseName.substr(4) + ".jpg"),
         position(0.0f),
         orientation(glm::angleAxis(glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f))), // Rotate 180° around Z to orient the number plate correctly
         scale(0.06f)                                                                // scale to match table dimensions
   {
   }

   Ball::~Ball()
   {
   }

   void Ball::Install()
   {
      if (!meshInitialized)
      {
         loadMesh();     // load vertices, uvs, normals and material
         setupBuffers(); // upload interleaved data (pos, uv, normal)
         meshInitialized = true;
      }
      // Each instance loads its own texture
      loadTexture();
   }

   void Ball::Render(GLuint shaderProgram,
                     const glm::mat4 &view,
                     const glm::mat4 &projection) const
   {
      // Compute Model matrix
      glm::mat4 model = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(orientation) * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
      // Compute MVP
      glm::mat4 mvp = projection * view * model;
      // Send MVP uniform
      GLint loc = glGetUniformLocation(shaderProgram, "MVP");
      glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

      // Bind this instance's texture to unit 0
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, textureID);

      // Draw interleaved mesh
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
      glBindVertexArray(0);
   }

   void Ball::loadMesh()
   {
      // Load mesh + material from OBJ/MTL
      Material mat;
      if (!LoadObject(objPath.c_str(), vertices, uvs, normals, mat))
      {
         std::cerr << "Failed to load OBJ: " << objPath << "\n";
         return;
      }
      // Store the loaded material for use in shaders later
      this->material = mat;
   }

   void Ball::setupBuffers()
   {
      // Interleaved vertex struct: pos, uv, normal
      struct V
      {
         glm::vec3 p;
         glm::vec2 uv;
         glm::vec3 n;
      };
      std::vector<V> data;
      data.reserve(vertices.size());
      for (size_t i = 0; i < vertices.size(); ++i)
      {
         // Flip U coordinate to correct horizontal mirroring of numbers
         glm::vec2 flippedUV(1.0f - uvs[i].x, uvs[i].y);
         data.push_back({vertices[i], flippedUV, normals[i]});
      }

      // Generate and bind VAO/VBO once
      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);

      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER,
                   data.size() * sizeof(V),
                   data.data(),
                   GL_STATIC_DRAW);

      // position at layout 0
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                            sizeof(V),
                            (void *)offsetof(V, p));
      glEnableVertexAttribArray(0);
      // uv at layout 1
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                            sizeof(V),
                            (void *)offsetof(V, uv));
      glEnableVertexAttribArray(1);
      // normal at layout 2
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                            sizeof(V),
                            (void *)offsetof(V, n));
      glEnableVertexAttribArray(2);

      glBindVertexArray(0);
   }

   void Ball::loadTexture()
   {
      int w, h, n;
      unsigned char *data = stbi_load(texPath.c_str(), &w, &h, &n, STBI_rgb);
      if (!data)
      {
         std::cerr << "Failed to load texture: " << texPath << "\n";
         return;
      }
      glGenTextures(1, &textureID);
      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D,
                   0, GL_RGB, w, h,
                   0, GL_RGB, GL_UNSIGNED_BYTE,
                   data);
      glGenerateMipmap(GL_TEXTURE_2D);
      stbi_image_free(data);
   }

} // namespace P3D