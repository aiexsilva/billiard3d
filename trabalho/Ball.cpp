#include "Ball.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

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
         texPath("PoolBalls/PoolBalluv" + baseName.substr(4) + ".jpg")
   {
   }

   Ball::~Ball()
   {
   }

   void Ball::Install()
   {
      if (!meshInitialized)
      {
         loadMesh();
         setupBuffers();
         meshInitialized = true;
      }
      // Each instance loads its own texture
      loadTexture();
   }

   void Ball::Render(GLuint shaderProgram,
                     const glm::mat4 &view,
                     const glm::mat4 &projection) const
   {
      // Calculate Model–View–Projection matrix
      glm::mat4 model = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(orientation) * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
      glm::mat4 mvp = projection * view * model;
      GLint loc = glGetUniformLocation(shaderProgram, "MVP");
      glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

      // Bind this instance's texture to unit 0
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, textureID);

      // Draw interleaved mesh
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
      glBindVertexArray(0);
   }

   void Ball::loadMesh()
   {
      // Load positions, UVs, normals from OBJ
      if (!LoadObject(objPath.c_str(), vertices, uvs, normals))
         std::cerr << "Failed to load OBJ: " << objPath << "\n";
   }

   void Ball::setupBuffers()
   {
      // Create interleaved array of {pos, uv}
      struct V
      {
         glm::vec3 p;
         glm::vec2 uv;
      };
      std::vector<V> data;
      data.reserve(vertices.size());
      for (size_t i = 0; i < vertices.size(); ++i)
         data.push_back({vertices[i], uvs[i]});

      // Generate VAO and VBO once
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