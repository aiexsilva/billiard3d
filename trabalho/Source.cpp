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
#include "Ball.h"

using namespace std;

#define height 800
#define width 1200

RenderProcessing::RenderPro tableRender;
vector<Ball::Ball> balls(16);

const int minimapSize = 400;

glm::vec3 tableRUcorner(0.5f, 0.15f, 1.0f);
glm::vec3 tableRDcorner(0.5f, -0.15f, 1.0f);
glm::vec3 tableLUcorner(-0.5f, 0.15f, 1.0f);
glm::vec3 tableLDcorner(-0.5f, -0.15f, 1.0f);

int main()
{
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_DEPTH_BITS, 24); 

   GLFWwindow *window = glfwCreateWindow(width, height, "Billiards", NULL, NULL);
   if (!window)
   {
      std::cout << "failed to create window\n";
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);

   glfwSetCursorPosCallback(window, Controls::cursor_callback);
   glfwSetScrollCallback(window, Controls::scroll_callback);
   glfwSetMouseButtonCallback(window, Controls::mouse_button_callback);

   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   glewExperimental = GL_TRUE;
   if (glewInit() != GLEW_OK)
   {
      std::cout << "failed to initialize glew\n";
      return -1;
   }

   glEnable(GL_DEPTH_TEST);
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

   GLfloat paraleli_vertices[] = {
       -0.5f, -0.15f, 1.0f,
       0.5f, -0.15f, 1.0f,  
       0.5f, 0.15f, 1.0f,   
       -0.5f, 0.15f, 1.0f,  

       -0.5f, -0.15f, -1.0f, 
       0.5f, -0.15f, -1.0f,  
       0.5f, 0.15f, -1.0f,  
       -0.5f, 0.15f, -1.0f  
   };

   GLuint paraleli_indices[] = {
       0, 1, 2, 0, 2, 3,
       4, 5, 6, 4, 6, 7,
       0, 3, 7, 0, 7, 4,
       1, 2, 6, 1, 6, 5,
       2, 3, 7, 2, 7, 6,
       0, 1, 5, 0, 5, 4};

   vector<glm::vec3> tableVerts;
   for (int i = 0; i < 36; ++i)
   {
      int idx = paraleli_indices[i];
      glm::vec3 v(
          paraleli_vertices[idx * 3 + 0],
          paraleli_vertices[idx * 3 + 1],
          paraleli_vertices[idx * 3 + 2]);
      tableVerts.push_back(v);
   }

   ShaderInfo table_shader[] = {
       {GL_VERTEX_SHADER, "table.vert"},
       {GL_FRAGMENT_SHADER, "table.frag"},
       {GL_NONE, NULL}};
   ShaderInfo ball_shader[] = {
       {GL_VERTEX_SHADER, "ball.vert"}, 
       {GL_FRAGMENT_SHADER, "ball.frag"},
       {GL_NONE, NULL}};

   GLuint tableShader = LoadShaders(table_shader);
   if (!tableShader)
      exit(EXIT_FAILURE);
   GLuint ballShader = LoadShaders(ball_shader);
   if (!ballShader)
      exit(EXIT_FAILURE);

   tableRender.ManualLoad(tableVerts);

   vector<glm::vec3> tableColors;
   tableColors.reserve(tableVerts.size());
   glm::vec3 faceGreen[6] = {
       {0.2f, 0.8f, 0.2f},
       {0.1f, 0.6f, 0.1f},
       {0.15f, 0.75f, 0.15f},
       {0.25f, 0.85f, 0.25f},
       {0.3f, 0.9f, 0.3f},
       {0.05f, 0.5f, 0.05f}};
   for (int face = 0; face < 6; ++face)
   {
      for (int i = 0; i < 6; ++i)
      {
         tableColors.push_back(faceGreen[face]);
      }
   }

   tableRender.SetTableColors(tableColors);

   tableRender.Install();
   tableRender.Set(tableShader);

   for (int i = 0; i < balls.size(); i++)
   {
      RenderProcessing::RenderPro ballRenderPro;

      balls[i].getRenderPro(ballRenderPro);
      ballRenderPro.Set(ballShader);
      ballRenderPro.Load("PoolBalls/Ball" + std::to_string(i + 1) + ".obj");
      ballRenderPro.Install();
      ballRenderPro.SetScale(glm::vec3(0.05f));
      balls[i].setRenderPro(ballRenderPro);
   }

   float r = balls[0].getRadius();
   float spacing = r * 3.0f * 1.1f;
   float tableY = 1.0f;

   glm::vec3 rackCenter = glm::vec3(0.0f, tableY, 1.2f);
   int idx = 0;
   for (int row = 0; row < 5; ++row)
   {
      int ballsInRow = row + 1;
      float startX = rackCenter.x - (ballsInRow - 1) * spacing * 0.5f;
      float z = rackCenter.z - row * spacing;
      for (int j = 0; j < ballsInRow; ++j)
      {
         float x = startX + j * spacing;
         balls[idx].setPosition({x, tableY, z});
         balls[idx].setOrientation({0.0f, 1.0f, 0.0f});
         ++idx;
      }
   }

   balls[15].setPosition({0.0f, tableY, 22.5f});
   balls[15].setOrientation({0, 1, 0});

   double lastTime = glfwGetTime();

   while (!glfwWindowShouldClose(window))
   {
      Controls::handleInput(window, balls);

      glDisable(GL_SCISSOR_TEST);

      int fbW, fbH;
      glfwGetFramebufferSize(window, &fbW, &fbH);
      glViewport(0, 0, fbW, fbH);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glm::vec3 tablePosition(0.0f, -0.12f, 0.0f);
      glm::vec3 tableOrientation(0.0f, 0.0f, 0.0f);
      tableRender.SetScale(glm::vec3(1.5f));
      tableRender.Render(tablePosition, tableOrientation);

      double currentTime = glfwGetTime();
      float deltaTime = float(currentTime - lastTime);
      lastTime = currentTime;
      deltaTime = std::min(deltaTime, 1.0f / 60.0f);

      for (auto &ball : balls)
      {
         ball.update(deltaTime, glm::vec2(-14.0f, -29.0f), glm::vec2(14.0f, 29.0f));

         RenderProcessing::RenderPro ballMainRender;
         ball.getRenderPro(ballMainRender);
         ballMainRender.modelRotation = tableRender.modelRotation;
         ballMainRender.SetWindow(window);

         ballMainRender.Render(ball.getPosition(), ball.getOrientation());
      }

      for (size_t i = 0; i < balls.size(); ++i)
      {
         for (size_t j = i + 1; j < balls.size(); ++j)
         {
            balls[i].handleBallCollision(balls[j]);
         }
      }

      int minix = fbW - minimapSize - 10;
      int miniy = fbH - minimapSize - 10;
      const int border = 1;

      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glEnable(GL_SCISSOR_TEST);
      glScissor(
          minix - border,
          miniy - border,
          minimapSize + 2 * border,
          minimapSize + 2 * border);
      glClear(GL_COLOR_BUFFER_BIT);

      glScissor(minix, miniy, minimapSize, minimapSize);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDisable(GL_SCISSOR_TEST);

      glViewport(minix, miniy, minimapSize, minimapSize);

      tableRender.RenderInMinimap(tablePosition, tableOrientation);
      tableRender.SetScale(glm::vec3(1.5f));
      for (auto &ball : balls)
      {
         RenderProcessing::RenderPro ballMainRender;
         ball.getRenderPro(ballMainRender);
         ballMainRender.RenderInMinimap(ball.getPosition(), ball.getOrientation());
      }

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   glfwTerminate();
   return 0;
}