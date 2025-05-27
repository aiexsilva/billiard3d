#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ObjLoader.h"

<<<<<<< Updated upstream
namespace RenderProcessing {

class RenderPro {
public:
    void ManualLoad(const std::vector<glm::vec3>& Vertices);
    bool Load(const std::string filepath);
    void Install();
    void Set(GLuint shader);
    void Render(glm::vec3 position, glm::vec3 rotation);
    void SetScale(glm::vec3 newScale);
    GLuint LoadTexture(const std::string filepath);

private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    glm::vec3 scale = glm::vec3(1.0f);

    Material currentMaterial; 

    GLuint shader;
    GLuint textureLoc; 
    GLuint texture = 0;     
    GLuint VAO, VBO;
    GLuint mvpLoc;
};

=======
namespace RenderProcessing
{
    class RenderPro
    {
    public:
        void ManualLoad(const std::vector<glm::vec3> &Vertices);
        bool Load(const std::string filepath);
        void Install();
        void Set(GLuint shader);
        void Render(glm::vec3 position, glm::vec3 rotation);
        void RenderInMinimap(glm::vec3 position, glm::vec3 rotation);
        void SetScale(glm::vec3 newScale);
        void processInput(GLFWwindow* window);
        void SetWindow(GLFWwindow* window);
        GLuint LoadTexture(const std::string filepath);

        glm::vec3 modelRotation = glm::vec3(0.0f);
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);

    private:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        glm::vec3 scale = glm::vec3(1.0f);
        Material currentMaterial;
        GLuint shader;
        GLuint textureLoc;
        GLuint texture = 0;
        GLuint VAO, VBO;
        GLuint mvpLoc;
        GLuint modelLoc;
        GLuint viewLoc;
        GLuint modelViewLoc;
        GLFWwindow* window = nullptr;
    };
>>>>>>> Stashed changes
}
