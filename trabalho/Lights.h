#ifndef Lights_h
#define Lights_h

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace Lights{
    class AmbientLight
    {
    public:
        void AmbientL();

        void setAmbientColor(float r, float g, float b);
        void setAmbientIntensity(float intensity);

        glm::vec3 getAmbientColor();
        float getAmbientIntensity();
        float ambientIntensity;
        int ambientColor;
    };
}

#endif