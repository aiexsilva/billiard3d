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
#include "Controls.h"

using namespace std;
using namespace Controls;

#define height 800
#define width 1200
//#define GLEW_STATIC
RenderProcessing::RenderPro tableRender;
RenderProcessing::RenderPro ballRender;

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

    //using functions from controls.cpp to control cursor and scroll 
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //hides cursor
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    std::vector<glm::vec3> tableVerts;

    for (int i = 0; i < 36; ++i) {
        int idx = paraleli_indices[i];
        glm::vec3 v(paraleli_vertices[idx * 3 + 0],paraleli_vertices[idx * 3 + 1],paraleli_vertices[idx * 3 + 2]);
        tableVerts.push_back(v);
    }

    ShaderInfo table_shader[] = {
     { GL_VERTEX_SHADER,   "table.vert" },
     { GL_FRAGMENT_SHADER, "table.frag" },
     { GL_NONE, NULL }
    };

    ShaderInfo ball_shader[] = {
     { GL_VERTEX_SHADER,   "ball.vert" },
     { GL_FRAGMENT_SHADER, "ball.frag" },
     { GL_NONE, NULL }
    };

    GLuint tableShader = LoadShaders(table_shader);
    if (!tableShader) exit(EXIT_FAILURE);

    GLuint ballShader = LoadShaders(ball_shader);
    if (!ballShader) exit(EXIT_FAILURE);

    tableRender.ManualLoad(tableVerts);
    tableRender.Install();
    tableRender.Set(tableShader);

    ballRender.Load("PoolBalls/Ball1.obj");
    ballRender.Install();
    ballRender.Set(ballShader);

    //depth testing
    glEnable(GL_DEPTH_TEST);

    //main loop
    while (!glfwWindowShouldClose(window)) {
        handleInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 tablePosition(0.0f, -5.0f, -2.0f);  
        glm::vec3 tableRotation(0.0f, 0.0f, 0.0f);   
        // tableRender.Render(tablePosition, tableRotation);

        glm::vec3 ballPosition(0.0f, 3.0f, -2.0f);  
        glm::vec3 ballRotation(0.0f, 0.0f, 0.0f);  
        ballRender.SetScale(glm::vec3(0.05f));
        ballRender.Render(ballPosition, ballRotation);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
