#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>

#include "ShaderLoader.h" 

using namespace std;

#define height 800
#define width 1200
#define GLEW_STATIC

int main() {
    //init (these 5 lines are specifically needed for macos)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

    //square vertices
    float vertices[] = {
        // X, Y
        -0.5f,  0.5f,  //vertex 1
         0.5f,  0.5f,  //vertex 2
         0.5f, -0.5f,  //vertex 3
        -0.5f, -0.5f   //vertex 4
    };

    //main loop
    while (!glfwWindowShouldClose(window)) {
        
        glfwPollEvents();
    }

    //clean
    glfwTerminate();

    return 0;
}
