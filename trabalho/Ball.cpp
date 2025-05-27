#include "Ball.h"

namespace Ball
{

   Ball::Ball()
       : position(0.0f, 0.0f, 0.0f),
         orientation(0.0f, 0.0f, 0.0f),
         renderPro()
   {
   }

   Ball::~Ball()
   {
   }

   void Ball::setPosition(glm::vec3 position)
   {
      this->position = position;
   }

   void Ball::setOrientation(glm::vec3 orientation)
   {
      this->orientation = orientation;
   }

   void Ball::setRenderPro(RenderProcessing::RenderPro renderPro)
   {
      this->renderPro = renderPro;
   }


   glm::vec3 Ball::getPosition() 
   {
      return this->position;
   }
   glm::vec3 Ball::getOrientation()
   {
      return this->orientation;
   }
   void Ball::getRenderPro(RenderProcessing::RenderPro &renderPro)
   {
      renderPro = this->renderPro;
   }


}