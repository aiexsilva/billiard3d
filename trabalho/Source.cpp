#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderLoader.h" 
#include "ObjLoader.h"

using namespace std;

#define height 800
#define width 1200
#define GLEW_STATIC

#define NumVAOs 1
#define NumBuffers 2

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

//initiaize vectors to store info obtained from objloader.cpp 
std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals;

int minimap = 200;

int main() {
    //init (these 5 lines are specifically needed for macos)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24); //u need this to enable the depth test

    //create window
    GLFWwindow* window = glfwCreateWindow(width, height, "Billiards", NULL, NULL);
    if (!window) {
        std::cout << "failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //init GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "failed to initialize glew\n";
        return -1;
    }

    //3d parallelepiped vertices, assuming width = 1, lenght = 2, height = 0.3 
    GLfloat paraleli_vertices[] = {
        //front 
        -0.5f, -0.15f, 1.0f,  //front bottom left
        0.5f, -0.15f, 1.0f,   //front bottom right
        0.5f, 0.15f, 1.0f,    //front top right
        -0.5f, 0.15f, 1.0f,   //front top left
    
        //back 
        -0.5f, -0.15f, -1.0f,  //back bottom left
        0.5f, -0.15f, -1.0f,   //back bottom right
        0.5f, 0.15f, -1.0f,    //back top right
        -0.5f, 0.15f, -1.0f    //back top left
    };

    GLuint paraleli_indices[] = {
        //front face
        0, 1, 2, 0, 2, 3,
    
        //back face
        4, 5, 6, 4, 6, 7,
    
        //left face
        0, 3, 7, 0, 7, 4,
    
        //right face
        1, 2, 6, 1, 6, 5,
    
        //top face
        2, 3, 7, 2, 7, 6,
    
        //bottom face
        0, 1, 5, 0, 5, 4
    };

    glGenVertexArrays(NumVAOs, VAOs);
    glGenBuffers(NumBuffers, Buffers);

    GLuint shader = LoadShaders("table.vert", "table.frag");
    GLuint shader2 = LoadShaders("ball.vert", "ball.frag");

    bool obj = LoadObject("PoolBalls/Ball1.obj", vertices, uvs, normals);
    if (!obj) {
    std::cout << "unable to load obj file\n";
    } else {
    std::cout << "loaded " << vertices.size() << " vertices from obj file\n";
    }

    GLuint objVAO, objVBO;
    glGenVertexArrays(1, &objVAO);
    glGenBuffers(1, &objVBO);

    //identity matrix
    glm::mat4 model = glm::mat4(1.0f);
    //camera position, where the camera is looking at, up direction on y axis
    glm::mat4 view = glm::lookAt(glm::vec3(1.5f, 1.5f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //angle which gives u more or less zoom, 0.1 and 100 define that objects closer or further than that will not be rendered
    glm::mat4 projection = glm::perspective(glm::radians(35.0f), (float)width / height, 0.1f, 100.0f);
    glm::mat4 mvp = projection * view * model;

    GLuint mvpLoc = glGetUniformLocation(shader, "MVP");
    GLuint mvpLoc2 = glGetUniformLocation(shader2, "MVP2");

    //--table--
    glUseProgram(shader);
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    //bind
    glBindVertexArray(VAOs[0]);

    //buffers for 3d parallelepiped
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(paraleli_vertices), paraleli_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(paraleli_indices), paraleli_indices, GL_STATIC_DRAW);

     //position for 3d parallelepiped
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    //--ball--
    glUseProgram(shader2);
    glUniformMatrix4fv(mvpLoc2, 1, GL_FALSE, glm::value_ptr(mvp));
    
    //bind
    glBindVertexArray(objVAO);

    //buffers for ball
    glBindBuffer(GL_ARRAY_BUFFER, objVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    //position for ball
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    //depth testing
    glEnable(GL_DEPTH_TEST);

    //main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //render table
        glm::mat4 tableModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));  
        glm::mat4 tableMVP = projection * view * tableModel;
        glUseProgram(shader);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(tableMVP));

        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //render ball
        glm::mat4 ballModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f)); 
        ballModel = glm::scale(ballModel, glm::vec3(0.05f, 0.05f, 0.05f));
        glm::mat4 ballMVP = projection * view * ballModel;
        glUseProgram(shader2);
        glUniformMatrix4fv(mvpLoc2, 1, GL_FALSE, glm::value_ptr(ballMVP));

        glBindVertexArray(objVAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
