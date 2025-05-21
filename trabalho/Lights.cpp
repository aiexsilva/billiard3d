#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <glm/glm.hpp>

#include "Lights.h"

namespace Lights {
    // Function to apply ambient light 
    void AmbientL()
    {
        // Define ambient color (RGBA)
        GLfloat ambientColor[] = {0.3f, 0.3f, 0.3f, 1.0f}; // Soft white ambient light

        // Enable lighting system
        glEnable(GL_LIGHTING);

        // Set the ambient light color for LIGHT0
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);

        // Enable LIGHT0
        glEnable(GL_LIGHT0);
    }
}