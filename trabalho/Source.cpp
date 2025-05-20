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

// 1 is true, 0 is false, change these to enable/disable camera controls
#define ROTATE_CAMERA 1
#define ZOOM_CAMERA 1

#define height 800
#define width 1200

RenderProcessing::RenderPro tableRender;

vector<Ball::Ball> balls(15);

// size of the minimap in pixels
const int minimapSize = 400;

glm::vec3 tableRUcorner(0.5f, 0.15f, 1.0f);
glm::vec3 tableRDcorner(0.5f, -0.15f, 1.0f);
glm::vec3 tableLUcorner(-0.5f, 0.15f, 1.0f);
glm::vec3 tableLDcorner(-0.5f, -0.15f, 1.0f);

int main()
{
   // init (these 5 lines are specifically needed for macos)
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_DEPTH_BITS, 24); // you need this to enable the depth test

   // create window
   GLFWwindow *window = glfwCreateWindow(width, height, "Billiards", NULL, NULL);
   if (!window)
   {
      std::cout << "failed to create window\n";
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);

   // using functions from controls.cpp to control cursor and scroll
   if (ROTATE_CAMERA)
      glfwSetCursorPosCallback(window, Controls::cursor_callback);
   if (ZOOM_CAMERA)
      glfwSetScrollCallback(window, Controls::scroll_callback);

   // hides cursor
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   // init GLEW
   glewExperimental = GL_TRUE;
   if (glewInit() != GLEW_OK)
   {
      std::cout << "failed to initialize glew\n";
      return -1;
   }

   // depth testing
   glEnable(GL_DEPTH_TEST);
   // set a default clear color (background)
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

   // 3d parallelepiped vertices, assuming width = 1, length = 2, height = 0.3
   GLfloat paraleli_vertices[] = {
       // front
       tableLDcorner.x, tableLDcorner.y, tableLDcorner.z, // front bottom left
       tableRDcorner.x, tableRDcorner.y, tableRDcorner.z, // front bottom right
       tableRUcorner.x, tableRUcorner.y, tableRUcorner.z, // front bottom right
       tableLUcorner.x, tableLUcorner.y, tableLUcorner.z, // front bottom right

       // back
       -0.5f, -0.15f, -1.0f, // back bottom left
       0.5f, -0.15f, -1.0f,  // back bottom right
       0.5f, 0.15f, -1.0f,   // back top right
       -0.5f, 0.15f, -1.0f   // back top left
   };

   GLuint paraleli_indices[] = {
       // front face
       0, 1, 2, 0, 2, 3,
       // back face
       4, 5, 6, 4, 6, 7,
       // left face
       0, 3, 7, 0, 7, 4,
       // right face
       1, 2, 6, 1, 6, 5,
       // top face
       2, 3, 7, 2, 7, 6,
       // bottom face
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
   tableRender.Install();
   tableRender.Set(tableShader);

   for (size_t i = 0; i < balls.size(); ++i)
   {
      RenderProcessing::RenderPro rp;
      rp.Set(ballShader);
      rp.Load("PoolBalls/Ball" + std::to_string(i + 1) + ".obj");
      rp.Install();
      rp.SetScale(glm::vec3(0.05f));
      balls[i].setRenderPro(rp);
   }

   float r = balls[0].getRadius();
   float spacing = 2 * r + 2.5f;
   glm::vec2 minB{tableLDcorner.x, tableLDcorner.z};
   glm::vec2 maxB{tableRUcorner.x, tableRUcorner.z};
   glm::vec2 center = (minB + maxB) * 0.5f;
   float tableY = 1.0f;
   int base = 5;
   int idx = 0;
   for (int row = 0; row < base && idx < balls.size(); ++row)
   {
      int count = base - row;
      float startX = center.x - (count - 1) * spacing * 0.5f;
      float z = center.y - row * spacing;
      for (int c = 0; c < count && idx < balls.size(); ++c, ++idx)
      {
         float x = startX + c * spacing;
         balls[idx].setPosition({x, tableY, z});
         balls[idx].setRotation({0, 1, 0});
      }
   }

   double lastTime = glfwGetTime();

   // main loop
   while (!glfwWindowShouldClose(window))
   {
      Controls::handleInput(window, balls);

      // clear full screen
      glDisable(GL_SCISSOR_TEST);

      int fbW, fbH;
      glfwGetFramebufferSize(window, &fbW, &fbH);
      glViewport(0, 0, fbW, fbH);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // render main scene
      glm::vec3 tablePosition(0.0f, -0.12f, 0.0f);
      glm::vec3 tableRotation(0.0f, 0.0f, 0.0f);
      tableRender.SetScale(glm::vec3(1.5f));
      tableRender.Render(tablePosition, tableRotation);

      double currentTime = glfwGetTime();
      float deltaTime = float(currentTime - lastTime);
      lastTime = currentTime;
      deltaTime = std::min(deltaTime, 1.0f / 60.0f);

      for (auto &ball : balls)
      {
         ball.update(deltaTime, glm::vec2(-14.0f, -29.0f), glm::vec2(14.0f, 29.0f));
         RenderProcessing::RenderPro ballMainRender;
         ball.getRenderPro(ballMainRender);
         ballMainRender.Render(ball.getPosition(), ball.getRotation());
      }

      for (size_t i = 0; i < balls.size(); ++i)
      {
         for (size_t j = i + 1; j < balls.size(); ++j)
         {
            balls[i].handleBallCollision(balls[j]);
         }
      }

      // define minimap position
      int minix = fbW - minimapSize - 10;
      int miniy = fbH - minimapSize - 10;
      const int border = 1;

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
      tableRender.RenderInMinimap(tablePosition, tableRotation);
      tableRender.SetScale(glm::vec3(1.5f));
      for (auto &ball : balls)
      {
         RenderProcessing::RenderPro ballMainRender;
         ball.getRenderPro(ballMainRender);
         ballMainRender.RenderInMinimap(ball.getPosition(), ball.getRotation());
      }

      // swap buffers & poll events
      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   glfwTerminate();
   return 0;
}
