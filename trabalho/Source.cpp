// Source.cpp
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderLoader.h"
#include "Ball.hpp"

using namespace std;

// Window dimensions
const unsigned int WIDTH = 1200;
const unsigned int HEIGHT = 800;
const unsigned int MINIMAP_SIZE = 200;

// Table geometry VAO/VBO/EBO
GLuint VAO_table, VBO_table, EBO_table;

// Camera spherical coordinates for orbital view
float radius = 3.0f; // distance from table center
float yaw = 0.0f;    // horizontal angle
float pitch = 20.0f; // vertical angle
const float sensitivity = 0.1f;
const float zoomSpeed = 1.0f;
bool firstMouse = true;
float lastX = WIDTH * 0.5f;
float lastY = HEIGHT * 0.5f;

//--------------------------------------------------------------------------------
// Input & callback functions
//--------------------------------------------------------------------------------

// Close window on ESC key press
void handleInput(GLFWwindow *window)
{
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}

// Update yaw/pitch from mouse movement (orbit control)
void cursor_callback(GLFWwindow *window, double xpos, double ypos)
{
   if (firstMouse)
   {
      lastX = float(xpos);
      lastY = float(ypos);
      firstMouse = false;
   }
   float xoffset = float(xpos) - lastX;
   float yoffset = lastY - float(ypos);
   lastX = float(xpos);
   lastY = float(ypos);

   xoffset *= sensitivity;
   yoffset *= sensitivity;

   yaw += xoffset;
   pitch += yoffset;
   pitch = glm::clamp(pitch, -89.0f, 89.0f);
}

// Zoom in/out by adjusting radius (scroll wheel)
void scroll_callback(GLFWwindow *window, double /*xoffset*/, double yoffset)
{
   radius -= float(yoffset) * zoomSpeed;
   radius = glm::clamp(radius, 2.0f, 20.0f);
}

//--------------------------------------------------------------------------------
// Helper: create the table VAO/VBO/EBO
//--------------------------------------------------------------------------------
void setupTableGeometry()
{
   GLfloat verts[] = {
       // front
       -0.5f, -0.15f, 1.0f,
       0.5f, -0.15f, 1.0f,
       0.5f, 0.15f, 1.0f,
       -0.5f, 0.15f, 1.0f,
       // back
       -0.5f, -0.15f, -1.0f,
       0.5f, -0.15f, -1.0f,
       0.5f, 0.15f, -1.0f,
       -0.5f, 0.15f, -1.0f};
   GLuint idx[] = {
       0, 1, 2, 0, 2, 3, // front
       4, 5, 6, 4, 6, 7, // back
       0, 3, 7, 0, 7, 4, // left
       1, 2, 6, 1, 6, 5, // right
       2, 3, 7, 2, 7, 6, // top
       0, 1, 5, 0, 5, 4  // bottom
   };

   glGenVertexArrays(1, &VAO_table);
   glGenBuffers(1, &VBO_table);
   glGenBuffers(1, &EBO_table);

   glBindVertexArray(VAO_table);
   glBindBuffer(GL_ARRAY_BUFFER, VBO_table);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_table);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);
   glBindVertexArray(0);
}

//--------------------------------------------------------------------------------
// Main
//--------------------------------------------------------------------------------
int main()
{
   // Initialize GLFW
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
   glfwWindowHint(GLFW_DEPTH_BITS, 24);

   // Create window
   GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Billiards", nullptr, nullptr);
   if (!window)
   {
      cerr << "Failed to create GLFW window\n";
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);

   // Initialize GLEW
   glewExperimental = GL_TRUE;
   if (glewInit() != GLEW_OK)
   {
      cerr << "Failed to initialize GLEW\n";
      return -1;
   }

   // Register input callbacks
   glfwSetCursorPosCallback(window, cursor_callback);
   glfwSetScrollCallback(window, scroll_callback);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   // Load shaders
   GLuint tableShader = LoadShaders("table.vert", "table.frag");
   GLuint ballShader = LoadShaders("ball.vert", "ball.frag");

   // Tell ballShader that sampler2D uTex is texture unit 0
   glUseProgram(ballShader);
   glUniform1i(glGetUniformLocation(ballShader, "uTex"), 0);

   // Build table geometry once
   setupTableGeometry();

   // Prepare projection matrix (perspective)
   glm::mat4 projection = glm::perspective(
       glm::radians(60.0f),
       float(WIDTH) / float(HEIGHT),
       0.1f, 100.0f);

   glEnable(GL_DEPTH_TEST);

   // --- Set up the 8-ball rack with correct ordering ---

   // 1) Define the exact rack order (apex, alternating stripe/solid, 8-ball center, etc.)
   vector<string> rackOrder = {
       "Ball1",  // apex
       "Ball15", // row2 left  (stripe)
       "Ball2",  // row2 right (solid)

       "Ball13", // row3 left  (stripe)
       "Ball8",  // row3 center (8-ball)
       "Ball4",  // row3 right (solid)

       "Ball11", // row4 col0 (stripe)
       "Ball14", // row4 col1 (solid)
       "Ball5",  // row4 col2 (stripe)
       "Ball10", // row4 col3 (solid)

       "Ball7",  // row5 col0 (solid)
       "Ball12", // row5 col1 (stripe)
       "Ball3",  // row5 col2 (solid)
       "Ball9",  // row5 col3 (stripe)
       "Ball6"   // row5 col4 (solid)
   };

   // 2) Create exactly 15 Ball instances in that order, installing their textures
   vector<P3D::Ball> balls;
   balls.reserve(15);
   for (auto &name : rackOrder)
   {
      balls.emplace_back(name);
      balls.back().Install();
   }

   // 3) Compute equilateral-triangle positions
   float ballDiameter = 2.0f * balls[0].scale;
   float gap = 0.01f;
   float spacing = ballDiameter + gap;
   float y = 0.15f + balls[0].scale; // table + radius
   glm::vec3 origin(0.0f, y, -0.3f); // foot-spot

   int idx = 0;
   for (int row = 0; row < 5; ++row)
   {
      float z = origin.z - row * (spacing * sqrt(3.0f) / 2.0f);
      float rowWidth = row * spacing;
      for (int col = 0; col <= row; ++col)
      {
         float x = origin.x - rowWidth / 2.0f + col * spacing;
         balls[idx++].position = glm::vec3(x, y, z);
      }
   }

   // Uniform locations
   GLint tableMVPLoc = glGetUniformLocation(tableShader, "MVP");
   GLint ballMVPLoc = glGetUniformLocation(ballShader, "MVP");

   // Render loop
   while (!glfwWindowShouldClose(window))
   {
      handleInput(window);

      int fbW, fbH;
      glfwGetFramebufferSize(window, &fbW, &fbH);

      glViewport(0, 0, fbW, fbH);
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Build view matrix
      float yawRad = glm::radians(yaw);
      float pitchRad = glm::radians(pitch);
      glm::vec3 camPos = {
          radius * cos(pitchRad) * sin(yawRad),
          radius * sin(pitchRad),
          radius * cos(pitchRad) * cos(yawRad)};
      glm::mat4 view = glm::lookAt(
          camPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

      // Draw table
      glUseProgram(tableShader);
      glm::mat4 tableModel = glm::mat4(1.0f);
      glm::mat4 tableMVP = projection * view * tableModel;
      glUniformMatrix4fv(tableMVPLoc, 1, GL_FALSE, glm::value_ptr(tableMVP));
      glBindVertexArray(VAO_table);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

      // Draw balls
      glUseProgram(ballShader);
      for (size_t i = 0; i < balls.size(); ++i)
      {
         balls[i].Render(ballShader, view, projection);
      }

      // Draw minimap (same pattern)
      int minix = fbW - MINIMAP_SIZE - 10;
      int miniy = fbH - MINIMAP_SIZE - 10;
      const int border = 1;
      // white border
      glEnable(GL_SCISSOR_TEST);
      glScissor(minix - border, miniy - border,
                MINIMAP_SIZE + 2 * border, MINIMAP_SIZE + 2 * border);
      glClearColor(1, 1, 1, 1);
      glClear(GL_COLOR_BUFFER_BIT);
      // black background+depth
      glScissor(minix, miniy, MINIMAP_SIZE, MINIMAP_SIZE);
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDisable(GL_SCISSOR_TEST);
      glViewport(minix, miniy, MINIMAP_SIZE, MINIMAP_SIZE);

      glm::mat4 miniView = glm::lookAt(
          glm::vec3(0.0f, 3.0f, 0.0f),
          glm::vec3(0.0f),
          glm::vec3(0.0f, 0.0f, -1.0f));
      // table in minimap
      glUseProgram(tableShader);
      glUniformMatrix4fv(tableMVPLoc, 1, GL_FALSE,
                         glm::value_ptr(projection * miniView * tableModel));
      glBindVertexArray(VAO_table);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

      // balls in minimap
      glUseProgram(ballShader);
      for (size_t i = 0; i < balls.size(); ++i)
      {
         balls[i].Render(ballShader, miniView, projection);
      }

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   // Cleanup
   glDeleteVertexArrays(1, &VAO_table);
   glDeleteBuffers(1, &VBO_table);
   glDeleteBuffers(1, &EBO_table);
   glfwTerminate();
   return 0;
}