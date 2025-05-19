#include "Ball.h"

namespace Ball
{

   Ball::Ball()
       : position(0.0f, 0.0f, 0.0f),
         rotation(0.0f, 0.0f, 0.0f),
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

   void Ball::setRotation(glm::vec3 rotation)
   {
      this->rotation = rotation;
   }

   void Ball::setRenderPro(RenderProcessing::RenderPro renderPro)
   {
      this->renderPro = renderPro;
   }


   glm::vec3 Ball::getPosition() 
   {
      return this->position;
   }
   glm::vec3 Ball::getRotation()
   {
      return this->rotation;
   }
   void Ball::getRenderPro(RenderProcessing::RenderPro &renderPro)
   {
      renderPro = this->renderPro;
   }


}