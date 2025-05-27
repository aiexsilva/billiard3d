#ifndef BALL_H
#define BALL_H

#include <glm/glm.hpp>
#include "RenderProcessing.h"

namespace Ball
{
    class Ball
    {
    public:
        Ball();
        ~Ball();
        
        void setPosition(glm::vec3 position);
        void setOrientation(glm::vec3 rotation);

        glm::vec3 getPosition();
        glm::vec3 getOrientation();
        
        void setRenderPro(RenderProcessing::RenderPro renderPro);
        void getRenderPro(RenderProcessing::RenderPro &renderPro);

    private:
       glm::vec3 position;
       glm::vec3 orientation;
       RenderProcessing::RenderPro renderPro;
    };
}

#endif