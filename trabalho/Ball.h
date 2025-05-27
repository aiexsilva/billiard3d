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

      // physics related functions
      void setVelocity(glm::vec3 velocity);
      glm::vec3 getVelocity();

      void setMass(float mass);
      float getMass();

      void setRadius(float radius);
      float getRadius();

      void setFriction(float friction);
      float getFriction();

      void applyForce(glm::vec3 &force);
      void integrate(float deltaTime);

      void handleBallCollision(Ball &otherBall);
      void handleWallCollision(const glm::vec2 &minBounds, const glm::vec2 &maxBounds);

      void update(float deltaTime, const glm::vec2 &minBounds, const glm::vec2 &maxBounds);

   private:
      glm::vec3 position;
      glm::vec3 orientation;
      RenderProcessing::RenderPro renderPro;

      // physics related variables
      glm::vec3 accumulatedForces = glm::vec3(0.0f);
      glm::vec3 velocity = glm::vec3(0.0f);
      float radius = 1.0f;
      float mass = 1.0f;
      float friction = 0.4f;
      float restitution = 0.8f;
   };
}

#endif