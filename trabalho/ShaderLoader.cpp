#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>

#include "ShaderLoader.h" 

//read file
static const GLchar* ReadShader(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "failed to open file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
}
