// Ball.hpp
#ifndef BALL_HPP
#define BALL_HPP

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ObjLoader.h" // for P3D::Material

namespace P3D
{
   class Ball
   {
   public:
      Ball(const std::string &baseName);
      ~Ball();

      // Install mesh, buffers and texture
      void Install();

      // Render the ball given the camera view/projection
      void Render(GLuint shaderProgram,
                  const glm::mat4 &view,
                  const glm::mat4 &projection) const;

      // Public transform
      glm::vec3 position;
      glm::quat orientation;
      float scale = 1.0f;

   private:
      // Load geometry + material from OBJ/MTL
      void loadMesh();

      // Setup VAO/VBO with position, UV and normal
      void setupBuffers();

      // Load the diffuse texture
      void loadTexture();

      // Static mesh data shared by all instances
      static bool meshInitialized;
      static GLuint VAO;
      static GLuint VBO;
      static std::vector<glm::vec3> vertices;
      static std::vector<glm::vec2> uvs;
      static std::vector<glm::vec3> normals;

      // Instance data
      GLuint textureID;
      std::string objPath;
      std::string mtlPath;
      std::string texPath;

      // Material loaded from .mtl
      Material material;
   };
}

#endif // BALL_HPP
