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

// Window dimensions
const unsigned int WIDTH = 1200;
const unsigned int HEIGHT = 800;

// Minimap size in pixels
const unsigned int MINIMAP_SIZE = 200;

// Global VAO/VBO
GLuint VAOs[1];
GLuint Buffers[2];

// OBJ loader data
vector<glm::vec3> vertices;
vector<glm::vec2> uvs;
vector<glm::vec3> normals;

// Camera spherical coordinates for main view
float radius = 3.0f; // reduced to bring table closer into view
float yaw = 0.0f;
float pitch = 20.0f;

// Mouse control parameters
const float sensitivity = 0.1f;
const float zoomSpeed = 1.0f;
bool firstMouse = true;
float lastX = WIDTH * 0.5f;
float lastY = HEIGHT * 0.5f;

// Input handling: close window on ESC key
void handleInput(GLFWwindow *window)
{
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}

// Cursor position callback: update yaw and pitch based on mouse movement
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

// Scroll callback: zoom in/out by adjusting radius
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
   radius -= float(yoffset) * zoomSpeed;
   radius = glm::clamp(radius, 2.0f, 20.0f);
}

// Utility to setup VAO/VBO for mesh data
void makeVAO(const float *verts, size_t vertSize,
             const unsigned int *idx, size_t idxSize,
             GLuint &VAO, GLuint &VBO, GLuint &EBO)
{
   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   glGenBuffers(1, &EBO);
   glBindVertexArray(VAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, vertSize, verts, GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxSize, idx, GL_STATIC_DRAW);

   // position attribute
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);
   glBindVertexArray(0);
}

int main()
{
   // Initialize GLFW
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_DEPTH_BITS, 24);

   // Create window
   GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Billiards", nullptr, nullptr);
   if (!window)
   {
      cerr << "Failed to create GLFW window" << endl;
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);

   // Initialize GLEW
   glewExperimental = GL_TRUE;
   if (glewInit() != GLEW_OK)
   {
      cerr << "Failed to initialize GLEW" << endl;
      return -1;
   }

   // Register input callbacks
   glfwSetCursorPosCallback(window, cursor_callback);
   glfwSetScrollCallback(window, scroll_callback);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   // Load shaders
   GLuint tableShader = LoadShaders("table.vert", "table.frag");
   GLuint ballShader = LoadShaders("ball.vert", "ball.frag");

   // Load OBJ data
   if (!LoadObject("PoolBalls/Ball1.obj", vertices, uvs, normals))
   {
      cerr << "Failed to load OBJ file" << endl;
      return -1;
   }

   // Setup table geometry
   GLfloat tableVerts[] = {
       -0.5f, -0.15f, 1.0f,
       0.5f, -0.15f, 1.0f,
       0.5f, 0.15f, 1.0f,
       -0.5f, 0.15f, 1.0f,
       -0.5f, -0.15f, -1.0f,
       0.5f, -0.15f, -1.0f,
       0.5f, 0.15f, -1.0f,
       -0.5f, 0.15f, -1.0f};
   unsigned int tableIdx[] = {
       0, 1, 2, 0, 2, 3,
       4, 5, 6, 4, 6, 7,
       0, 3, 7, 0, 7, 4,
       1, 2, 6, 1, 6, 5,
       2, 3, 7, 2, 7, 6,
       0, 1, 5, 0, 5, 4};
   makeVAO(tableVerts, sizeof(tableVerts), tableIdx, sizeof(tableIdx), VAOs[0], Buffers[0], Buffers[1]);

   // Setup ball VAO/VBO
   GLuint ballVAO, ballVBO;
   glGenVertexArrays(1, &ballVAO);
   glGenBuffers(1, &ballVBO);
   glBindVertexArray(ballVAO);
   glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
   glEnableVertexAttribArray(0);
   glBindVertexArray(0);

   // Get uniform locations
   GLint tableMVPLoc = glGetUniformLocation(tableShader, "MVP");
   GLint ballMVPLoc = glGetUniformLocation(ballShader, "MVP2");

   // Setup projection matrix
   glm::mat4 projection = glm::perspective(glm::radians(60.0f), float(WIDTH) / HEIGHT, 0.1f, 100.0f);

   glEnable(GL_DEPTH_TEST);

   // Main render loop
   while (!glfwWindowShouldClose(window))
   {
      handleInput(window);

      // Get actual framebuffer size for HiDPI support
      int fbWidth, fbHeight;
      glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

      // Clear full framebuffer
      glViewport(0, 0, fbWidth, fbHeight);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Compute camera for main view
      float yawRad = glm::radians(yaw);
      float pitchRad = glm::radians(pitch);
      glm::vec3 camPos(
          radius * cos(pitchRad) * sin(yawRad),
          radius * sin(pitchRad),
          radius * cos(pitchRad) * cos(yawRad));
      glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

      // Draw main table
      glUseProgram(tableShader);
      glm::mat4 tableModel = glm::mat4(1.0f);
      glm::mat4 tableMVP = projection * view * tableModel;
      glUniformMatrix4fv(tableMVPLoc, 1, GL_FALSE, glm::value_ptr(tableMVP));
      glBindVertexArray(VAOs[0]);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

      // Draw main ball
      glUseProgram(ballShader);
      glm::mat4 ballModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
      ballModel = glm::scale(ballModel, glm::vec3(0.05f));
      glm::mat4 ballMVP = projection * view * ballModel;
      glUniformMatrix4fv(ballMVPLoc, 1, GL_FALSE, glm::value_ptr(ballMVP));
      glBindVertexArray(ballVAO);
      glDrawArrays(GL_TRIANGLES, 0, vertices.size());

      // Render minimap with white border and black background
      const int border = 1; // thickness in pixels
      int minix = fbWidth - MINIMAP_SIZE - 10;
      int miniy = fbHeight - MINIMAP_SIZE - 10;

      // 1. Clear border area to white using scissor
      glEnable(GL_SCISSOR_TEST);
      glScissor(minix - border, miniy - border,
                MINIMAP_SIZE + 2 * border, MINIMAP_SIZE + 2 * border);
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      // 2. Clear inner minimap area to black (also clear depth)
      glScissor(minix, miniy, MINIMAP_SIZE, MINIMAP_SIZE);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDisable(GL_SCISSOR_TEST);

      // 3. Set viewport to minimap region
      glViewport(minix, miniy, MINIMAP_SIZE, MINIMAP_SIZE);

      glm::mat4 miniView = glm::lookAt(
          glm::vec3(0.0f, 10.0f, 0.0f),
          glm::vec3(0.0f, 0.0f, 0.0f),
          glm::vec3(0.0f, 0.0f, -1.0f));

      // Draw table in minimap
      glUseProgram(tableShader);
      glUniformMatrix4fv(tableMVPLoc, 1, GL_FALSE,
                         glm::value_ptr(projection * miniView * tableModel));
      glBindVertexArray(VAOs[0]);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

      // Draw ball in minimap
      glUseProgram(ballShader);
      glUniformMatrix4fv(ballMVPLoc, 1, GL_FALSE,
                         glm::value_ptr(projection * miniView * ballModel));
      glBindVertexArray(ballVAO);
      glDrawArrays(GL_TRIANGLES, 0, vertices.size());

      // Swap buffers and poll events
      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   // Cleanup and exit
   glDeleteVertexArrays(1, VAOs);
   glDeleteBuffers(2, Buffers);
   glDeleteVertexArrays(1, &ballVAO);
   glDeleteBuffers(1, &ballVBO);
   glDeleteProgram(tableShader);
   glDeleteProgram(ballShader);
   glfwTerminate();
   return 0;
}