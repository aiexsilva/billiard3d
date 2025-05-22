#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderLoader.h"
#include "ObjLoader.h"
#include "RenderProcessing.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Controls.h"

#define HEIGHT 800
#define WIDTH 1200

bool useAmbient = false;
bool useDiffuse = false;
bool useSpecular = false;

namespace RenderProcessing
{

    void RenderPro::ManualLoad(const std::vector<glm::vec3> &Vertices)
    {
        vertices = Vertices;
    }

    GLuint RenderPro::LoadTexture(const std::string filepath)
    {
        int width, height, nrChannels;

        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
        if (!data)
        {
            std::cerr << "Failed to load texture: " << filepath << std::endl;
            return 0;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        return textureID;
    }

    bool RenderPro::Load(const std::string filepath)
    {
        std::string mtlfilename;
        bool obj = LoadObject(filepath, vertices, uvs, normals, mtlfilename);
        if (!obj)
        {
            std::cout << "unable to load obj file\n";
            return false;
        }
        std::cout << "loaded " << vertices.size() << " vertices from obj file\n";

        std::string mtlPath = "PoolBalls/" + mtlfilename;
        std::vector<Material> materials;
        bool mtlLoaded = LoadMTL(mtlPath, materials);
        if (!mtlLoaded)
        {
            std::cout << "unable to load mtl file\n";
            return false;
        }
        currentMaterial = materials[0];

        if (!currentMaterial.map_Kd.empty())
        {
            std::string texPath = "PoolBalls/" + currentMaterial.map_Kd;
            std::cout << "Texture map_Kd: " << currentMaterial.map_Kd << std::endl;
            texture = LoadTexture(texPath);
        }
        else
        {
            std::cout << "empty map_kd ";
        }

        return true;
    }

    void RenderPro::Install()
    {
        // -- position buffer --
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glEnableVertexAttribArray(0);

        // -- uv buffer object --
        GLuint VBO_UV;
        glGenBuffers(1, &VBO_UV);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
        glEnableVertexAttribArray(1);

        // -- normal buffer object --
        GLuint normalBuffer;
        glGenBuffers(1, &normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void RenderPro::Set(GLuint shader)
    {
        this->shader = shader;
        mvpLoc = glGetUniformLocation(shader, "MVP");
        modelLoc = glGetUniformLocation(shader, "model");
        textureLoc = glGetUniformLocation(shader, "textureSampler");
    }

    //move with keys
    void RenderPro::processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            modelRotation.y += 1.0f; // rotate around Y axis
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            modelRotation.y -= 1.0f; // rotate around Y axis
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            modelRotation.x += 1.0f; // rotate around X axi§s
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            modelRotation.x -= 1.0f; // rotate around X axis
        }
    }

    void RenderPro::Render(glm::vec3 position, glm::vec3 rotation)
    {
        glUseProgram(shader);
        //sends info to shader that variable uUseLighting is 1 or True
        glUniform1i(glGetUniformLocation(shader, "uUseLighting"), 1);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f); 

        //https://learnopengl.com/Lighting/Basic-Lighting light related stuff
        glUniform3fv(glGetUniformLocation(shader, "lightColor"), 1, glm::value_ptr(lightColor));
        //send to shader the currentmaterial.whatever to it's corresponding variable so it can be used in calculations
        glUniform3fv(glGetUniformLocation(shader, "uMaterial.ambient"), 1, glm::value_ptr(currentMaterial.Ka));
        glUniform3fv(glGetUniformLocation(shader, "uMaterial.diffuse"), 1, glm::value_ptr(currentMaterial.Kd));
        glUniform3fv(glGetUniformLocation(shader, "uMaterial.specular"), 1, glm::value_ptr(currentMaterial.Ks));
        glUniform1f(glGetUniformLocation(shader, "uMaterial.shininess"), currentMaterial.Ns);

        //sends camera position to shader to the viewPosition variable
        glUniform3fv(glGetUniformLocation(shader, "viewPosition"), 1, glm::value_ptr(cameraPos));

        //simple set up so u can switch lights using the keys 
        static bool lastKey1State = GLFW_RELEASE;
        static bool lastKey2State = GLFW_RELEASE;
        static bool lastKey3State = GLFW_RELEASE;

        if (window) {
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && lastKey1State == GLFW_RELEASE) {
                useAmbient = !useAmbient;
                glUniform1i(glGetUniformLocation(shader, "useAmbient"), useAmbient);
                lastKey1State = GLFW_PRESS; //key 1 pressed
            } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
                lastKey1State = GLFW_RELEASE;  //key 1 released
            }

            
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && lastKey2State == GLFW_RELEASE) {
                useDiffuse = !useDiffuse;
                glUniform1i(glGetUniformLocation(shader, "useDiffuse"), useDiffuse);
                lastKey2State = GLFW_PRESS;  //key 2 pressed
            } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
                lastKey2State = GLFW_RELEASE; 
            }

            
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && lastKey3State == GLFW_RELEASE) {
                useSpecular = !useSpecular;
                glUniform1i(glGetUniformLocation(shader, "useSpecular"), useSpecular);
                lastKey3State = GLFW_PRESS;  // key 3 pressed
            } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
                lastKey3State = GLFW_RELEASE; 
            }
        }

        //activate and bind texture to object
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureLoc, 0);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, scale);
        model = glm::translate(model, position);
        model = glm::translate(model, rotation);
        //these are used to rotate the objects however is subject to change
        model = glm::rotate(model, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));

        //view matrix 
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(35.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        //junction of all
        glm::mat4 mvp = projection * view * model;
        //sends model and mvp matrices to shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        //used vertex array stored in VAO binds it to the object 
        glBindVertexArray(VAO);
        //renders
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }

    void RenderPro::RenderInMinimap(glm::vec3 position, glm::vec3 rotation)
    {
        glUseProgram(shader);
        glUniform1i(glGetUniformLocation(shader, "uUseLighting"), 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureLoc, 0);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, scale);
        model = glm::translate(model, position);
        model = glm::translate(model, rotation);

        glm::vec3 camPos = glm::vec3(0.0f, 5.0f, 0.0f);
        glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 camUp = glm::vec3(0.0f, 0.0f, -1.0f);

        glm::mat4 view = glm::lookAt(camPos, camTarget, camUp);

        float orthoSize = 2.0f;
        glm::mat4 projection = glm::ortho(
            -orthoSize, orthoSize,  // left, right
            -orthoSize, orthoSize,  // bottom, top
            0.1f, 100.0f            // near, far
        );

        glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }

    void RenderPro::SetScale(glm::vec3 newScale)
    {
        scale = newScale;
    }

    void RenderPro::SetWindow(GLFWwindow* wins) {
        window = wins;
    }

}
