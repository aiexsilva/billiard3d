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

   void Ball::setVelocity(glm::vec3 velocity)
   {
      this->velocity = velocity;
   }
   glm::vec3 Ball::getVelocity()
   {
      return this->velocity;
   }
   void Ball::setMass(float mass)
   {
      this->mass = mass;
   }
   float Ball::getMass()
   {
      return this->mass;
   }
   void Ball::setRadius(float radius)
   {
      this->radius = radius;
   }
   float Ball::getRadius()
   {
      return this->radius;
   }
   void Ball::setFriction(float friction)
   {
      this->friction = friction;
   }
   float Ball::getFriction()
   {
      return this->friction;
   }

   void Ball::update(float deltaTime,
                     const glm::vec2 &minBounds,
                     const glm::vec2 &maxBounds)
   {
      glm::vec3 frictionForce = -friction * velocity;
      Ball::applyForce(frictionForce);
      Ball::integrate(deltaTime);
      Ball::handleWallCollision(minBounds, maxBounds);
   }

   void Ball::Ball::applyForce(glm::vec3 &force)
   {
      this->accumulatedForces += force;
   }

   void Ball::integrate(float deltaTime)
   {
      glm::vec3 frictionForce = -friction * velocity;
      accumulatedForces += frictionForce;

      glm::vec3 acceleration = accumulatedForces / mass;

      velocity += acceleration * deltaTime;
      position += velocity * deltaTime;

      accumulatedForces = glm::vec3(0.0f);
   }

   void Ball::handleWallCollision(const glm::vec2 &minBounds, const glm::vec2 &maxBounds)
   {
      float tempradius = radius / 10000;
      if (position.x - tempradius < minBounds.x && velocity.x < 0.0f)
      {
         position.x = minBounds.x + tempradius;
         velocity.x = -velocity.x * restitution;
      }
      else if (position.x + tempradius > maxBounds.x && velocity.x > 0.0f)
      {
         position.x = maxBounds.x - tempradius;
         velocity.x = -velocity.x * restitution;
      }

      if (position.z - tempradius < minBounds.y && velocity.z < 0.0f)
      {
         position.z = minBounds.y + tempradius;
         velocity.z = -velocity.z * restitution;
      }
      else if (position.z + tempradius > maxBounds.y && velocity.z > 0.0f)
      {
         position.z = maxBounds.y - tempradius;
         velocity.z = -velocity.z * restitution;
      }
   }

   void Ball::handleBallCollision(Ball &B)
   {
      glm::vec3 delta = B.position - position;
      float dist = glm::length(delta);
      float penetration = radius + B.radius - dist;
      if (penetration > 0.0f)
      {
         glm::vec3 delta = position - B.position;
         float dist = glm::length(delta);
         if (dist == 0.0f)
            return;
         glm::vec3 n = delta / dist;
         glm::vec3 vRel = velocity - B.velocity;
         float velAlongNormal = glm::dot(vRel, n);
         if (velAlongNormal > 0.0f)
            return;

         float invMassA = 1.0f / mass;
         float invMassB = 1.0f / B.mass;
         float e = std::min(restitution, B.restitution);
         float invMassSum = invMassA + invMassB;
         float j = -(1.0f + e) * velAlongNormal / invMassSum;
         glm::vec3 impulse = j * n;
         velocity += impulse / mass;
         B.velocity -= impulse / B.mass;

         const float percent = 0.8f;
         glm::vec3 correction = (penetration / invMassSum) * percent * n;
         position += correction * invMassA;
         B.position -= correction * invMassB;
      }
   }
}