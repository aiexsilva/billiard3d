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
<<<<<<< Updated upstream
=======
#include "Ball.h"
>>>>>>> Stashed changes

using namespace std;
using namespace Controls;

#define height 800
#define width 1200
//#define GLEW_STATIC
RenderProcessing::RenderPro tableRender;
RenderProcessing::RenderPro ballRender;

int minimap = 200;

<<<<<<< Updated upstream
int main() {
    //init (these 5 lines are specifically needed for macos)
=======
// size of the minimap in pixels
const int minimapSize = 400;

int main()
{
    // init (these 5 lines are specifically needed for macos)
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
    //init GLEW
=======
    // adicionado chamar evento de clik de botão do rato
    glfwSetMouseButtonCallback(window, Controls::mouse_button_callback);

    // hides cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // init GLEW
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
    //depth testing
    glEnable(GL_DEPTH_TEST);
=======
            RenderProcessing::RenderPro ballRenderPro;
            
            balls[j + (i * 4)].getRenderPro(ballRenderPro);

            ballRenderPro.Set(ballShader);
            ballRenderPro.Load("PoolBalls/Ball" + std::to_string(j + (i * 4) + 1) + ".obj");
            ballRenderPro.Install();
            ballRenderPro.SetScale(glm::vec3(0.05f));
            
            glm::vec3 ballPosition(-6.0f + i * 4, 1.0f, -6.0f + j * 4);
            glm::vec3 ballOrientation(0.0f, 1.0f, 1.0f);
            
            balls[j + (i * 4)].setPosition(ballPosition);
            balls[j + (i * 4)].setOrientation(ballOrientation);
            
            balls[j + (i * 4)].setRenderPro(ballRenderPro);
        }
    }

    // main loop
    while (!glfwWindowShouldClose(window))
    {
        Controls::handleInput(window);

        tableRender.processInput(window);

        /*// random animation for now
        for (int j = 0; j < balls.size(); j++)
        {
            glm::vec3 position = balls[j].getPosition();
            glm::vec3 offset(0.01f, 0.00f, 0.01f);
            position += offset;
            balls[j].setPosition(position);
        }*/

        // clear full screen
        glDisable(GL_SCISSOR_TEST);

        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        glViewport(0, 0, fbW, fbH);
>>>>>>> Stashed changes

    //main loop
    while (!glfwWindowShouldClose(window)) {
        handleInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

<<<<<<< Updated upstream
        glm::vec3 tablePosition(0.0f, -5.0f, -2.0f);  
        glm::vec3 tableRotation(0.0f, 0.0f, 0.0f);   
        // tableRender.Render(tablePosition, tableRotation);
=======
        // render main scene
        glm::vec3 tablePosition(0.0f, -0.12f, 0.0f);
        glm::vec3 tableOrientation(0.0f, 0.0f, 0.0f);
        tableRender.SetScale(glm::vec3(1.5f));
        tableRender.Render(tablePosition, tableOrientation);
        
        for(auto &ball : balls)
        {
            RenderProcessing::RenderPro ballMainRender;
            ball.getRenderPro(ballMainRender);
            ballMainRender.modelRotation = tableRender.modelRotation;
            ballMainRender.processInput(window);
            ballMainRender.SetWindow(window);
            ballMainRender.Render(ball.getPosition(), ball.getOrientation());
        }
>>>>>>> Stashed changes

        glm::vec3 ballPosition(0.0f, 3.0f, -2.0f);  
        glm::vec3 ballRotation(0.0f, 0.0f, 0.0f);  
        ballRender.SetScale(glm::vec3(0.05f));
        ballRender.Render(ballPosition, ballRotation);

<<<<<<< Updated upstream
=======
        // clear white border around the minimap
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_SCISSOR_TEST);
        glScissor(
            minix - border,
            miniy - border,
            minimapSize + 2 * border,
            minimapSize + 2 * border);
        glClear(GL_COLOR_BUFFER_BIT);

        // clear black inside minimap border
        glScissor(minix, miniy, minimapSize, minimapSize);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        // change viewport to minimap size
        glViewport(minix, miniy, minimapSize, minimapSize);

        // render table & balls in minimap
        tableRender.RenderInMinimap(tablePosition, tableOrientation);
        tableRender.SetScale(glm::vec3(1.5f));
        for(auto &ball : balls)
        {
            RenderProcessing::RenderPro ballMainRender;
            ball.getRenderPro(ballMainRender);
            ballMainRender.RenderInMinimap(ball.getPosition(), ball.getOrientation());
        }

        // swap buffers & poll events
>>>>>>> Stashed changes
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
