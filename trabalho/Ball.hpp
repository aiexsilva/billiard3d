#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GL/glew.h>
#include "ObjLoader.h"

namespace P3D
{

   //------------------------------------------------------------------------------
   // Ball.hpp
   // A class representing a pool ball mesh + material + texture, instantiated per-ball.
   // Uses ObjLoader to load .obj/.mtl and OpenGL for VAO/VBO + texture.
   //------------------------------------------------------------------------------

   class Ball
   {
   public:
      /// Construct a Ball pointing to files in PoolBalls/, named like "Ball1"
      /// Expects: PoolBalls/Ball1.obj, PoolBalls/Ball1.mtl, PoolBalls/PoolBalluv1.jpeg
      Ball(const std::string &baseName);
      ~Ball();

      /// Upload mesh, normals, uvs and texture to GPU (call once)
      void Install();

      /// Render the ball with given shader and camera matrices
      /// shaderProgram must have a uniform "MVP"
      void Render(GLuint shaderProgram,
                  const glm::mat4 &view,
                  const glm::mat4 &projection) const;

      // Per-instance transform
      glm::vec3 position{0.0f};
      glm::quat orientation{1, 0, 0, 0};
      float scale{0.05f};

   private:
      // Static mesh data shared by all Ball instances
      static bool meshInitialized;
      static GLuint VAO;
      static GLuint VBO;
      static GLuint EBO;
      static GLuint textureID;
      static std::vector<glm::vec3> vertices;
      static std::vector<glm::vec2> uvs;
      static std::vector<glm::vec3> normals;

      // Paths for this ball's resources
      std::string objPath;
      std::string mtlPath;
      std::string texPath;

      // Helpers
      void loadMesh();     // uses ObjLoader to fill vertices, uvs, normals
      void setupBuffers(); // glGenVAO/VBO/EBO and upload data
      void loadTexture();  // loads JPEG via stb_image (or other) into textureID
   };

} // namespace P3D
