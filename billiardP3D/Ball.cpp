#define _CRT_SECURE_NO_WARNINGS 

#include "Ball.h"  

namespace Ball
{

    // Construtor: inicializa a posi��o e orienta��o no (0,0,0) e o renderizador com valor padr�o
    Ball::Ball()
        : position(0.0f, 0.0f, 0.0f),
        orientation(0.0f, 0.0f, 0.0f),
        renderPro()
    {
    }

    // Destrutor vazio (nenhum recurso a liberar)
    Ball::~Ball()
    {
    }

    // Define a posi��o da bola
    void Ball::setPosition(glm::vec3 position)
    {
        this->position = position;
    }

    // Define a orienta��o (�ngulo ou rota��o)
    void Ball::setOrientation(glm::vec3 orientation)
    {
        this->orientation = orientation;
    }

    // Define o objeto de renderiza��o
    void Ball::setRenderPro(RenderProcessing::RenderPro renderPro)
    {
        this->renderPro = renderPro;
    }

    // Retorna a posi��o da bola
    glm::vec3 Ball::getPosition()
    {
        return this->position;
    }

    // Retorna a orienta��o da bola
    glm::vec3 Ball::getOrientation()
    {
        return this->orientation;
    }

    // Copia o objeto de renderiza��o
    void Ball::getRenderPro(RenderProcessing::RenderPro& renderPro)
    {
        renderPro = this->renderPro;
    }

    // Define a velocidade da bola
    void Ball::setVelocity(glm::vec3 velocity)
    {
        this->velocity = velocity;
    }

    // Retorna a velocidade
    glm::vec3 Ball::getVelocity()
    {
        return this->velocity;
    }

    // Define a massa da bola
    void Ball::setMass(float mass)
    {
        this->mass = mass;
    }

    // Retorna a massa
    float Ball::getMass()
    {
        return this->mass;
    }

    // Define o raio da bola
    void Ball::setRadius(float radius)
    {
        this->radius = radius;
    }

    // Retorna o raio
    float Ball::getRadius()
    {
        return this->radius;
    }

    // Define o coeficiente de atrito
    void Ball::setFriction(float friction)
    {
        this->friction = friction;
    }

    // Retorna o atrito
    float Ball::getFriction()
    {
        return this->friction;
    }

    // Atualiza o estado da bola com base no tempo passado e nos limites do ambiente
    void Ball::update(float deltaTime,
        const glm::vec2& minBounds,
        const glm::vec2& maxBounds)
    {
        // Aplica for�a de atrito contr�ria � velocidade
        glm::vec3 frictionForce = -friction * velocity;
        Ball::applyForce(frictionForce);

        // Integra o movimento com base nas for�as
        Ball::integrate(deltaTime);

        // Verifica colis�es com as paredes e ajusta
        Ball::handleWallCollision(minBounds, maxBounds);
    }

    // Aplica uma for�a acumulada � bola
    void Ball::Ball::applyForce(glm::vec3& force)
    {
        this->accumulatedForces += force;
    }

    // Calcula nova posi��o e velocidade usando f�sica cl�ssica (F = m * a)
    void Ball::integrate(float deltaTime)
    {
        // Reaplica for�a de atrito (redundante: j� foi feito em `update`)
        glm::vec3 frictionForce = -friction * velocity;
        accumulatedForces += frictionForce;

        // Calcula acelera��o: a = F / m
        glm::vec3 acceleration = accumulatedForces / mass;

        // Atualiza velocidade e posi��o
        velocity += acceleration * deltaTime;
        position += velocity * deltaTime;

        // Limpa as for�as acumuladas
        accumulatedForces = glm::vec3(0.0f);
    }

    // Verifica e trata colis�es com os limites do espa�o (paredes)
    void Ball::handleWallCollision(const glm::vec2& minBounds, const glm::vec2& maxBounds)
    {
        // Reduz o raio para considerar �rea de contato (escala muito pequena)
        float tempradius = radius; // / 10000;

        // Colis�o no eixo X - lado esquerdo
        if (position.x - tempradius < minBounds.x && velocity.x < 0.0f)
        {
            position.x = minBounds.x + tempradius;
            velocity.x = -velocity.x * restitution; // Inverte e aplica restitui��o (rebote)
        }
        // Colis�o no eixo X - lado direito
        else if (position.x + tempradius > maxBounds.x && velocity.x > 0.0f)
        {
            position.x = maxBounds.x - tempradius;
            velocity.x = -velocity.x * restitution;
        }

        // Colis�o no eixo Z - parte inferior
        if (position.z - tempradius < minBounds.y && velocity.z < 0.0f)
        {
            position.z = minBounds.y + tempradius;
            velocity.z = -velocity.z * restitution;
        }
        // Colis�o no eixo Z - parte superior
        else if (position.z + tempradius > maxBounds.y && velocity.z > 0.0f)
        {
            position.z = maxBounds.y - tempradius;
            velocity.z = -velocity.z * restitution;
        }
    }

    // Trata colis�o com outra bola
    void Ball::handleBallCollision(Ball& B)
    {
        glm::vec3 delta = B.position - position;
        float dist = glm::length(delta);
        float penetration = radius + B.radius - dist;

        // Verifica se h� colis�o (interpenetra��o)
        if (penetration > 0.0f)
        {
            // Recalcula normal de colis�o
            glm::vec3 delta = position - B.position;
            float dist = glm::length(delta);

            if (dist == 0.0f) return; // Evita divis�o por zero

            glm::vec3 n = delta / dist; // Vetor normal da colis�o

            glm::vec3 vRel = velocity - B.velocity; // Velocidade relativa

            float velAlongNormal = glm::dot(vRel, n); // Proje��o na dire��o da colis�o

            if (velAlongNormal > 0.0f) return; // Se est�o se afastando, ignora

            // Calcula impulsos baseados na massa
            float invMassA = 1.0f / mass;
            float invMassB = 1.0f / B.mass;

            // Usa o menor coeficiente de restitui��o entre as duas bolas
            float e = std::min(restitution, B.restitution);

            float invMassSum = invMassA + invMassB;

            // Calcula impulso escalar
            float j = -(1.0f + e) * velAlongNormal / invMassSum;
            glm::vec3 impulse = j * n;

            // Aplica o impulso inversamente proporcional � massa
            velocity += impulse / mass;
            B.velocity -= impulse / B.mass;

            // Corrige interpenetra��o (posicionamento) com 80% da penetra��o
            const float percent = 0.8f;
            glm::vec3 correction = (penetration / invMassSum) * percent * n;

            position += correction * invMassA;
            B.position -= correction * invMassB;
        }
    }
}
