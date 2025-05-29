#ifndef BALL_H
#define BALL_H  

#include <glm/glm.hpp>              
#include "RenderProcessing.h"       

namespace Ball
{
    // Defini��o da classe Ball (bola)
    class Ball
    {
    public:
        // Construtor e destrutor
        Ball();     // Inicializa valores padr�o
        ~Ball();    // Libera recursos (se necess�rio)

        // Define a posi��o da bola no espa�o 3D
        void setPosition(glm::vec3 position);

        // Define a orienta��o (rotacionamento) da bola
        void setOrientation(glm::vec3 rotation);

        // Obt�m a posi��o atual
        glm::vec3 getPosition();

        // Obt�m a orienta��o atual
        glm::vec3 getOrientation();

        // Define o objeto de renderiza��o da bola
        void setRenderPro(RenderProcessing::RenderPro renderPro);

        // Copia os dados de renderiza��o para outro objeto
        void getRenderPro(RenderProcessing::RenderPro& renderPro);

        // ======= Fun��es relacionadas � f�sica =======

        // Define a velocidade da bola
        void setVelocity(glm::vec3 velocity);

        // Obt�m a velocidade atual
        glm::vec3 getVelocity();

        // Define a massa da bola
        void setMass(float mass);

        // Retorna a massa da bola
        float getMass();

        // Define o raio da bola
        void setRadius(float radius);

        // Retorna o raio da bola
        float getRadius();

        // Define o coeficiente de atrito
        void setFriction(float friction);

        // Retorna o coeficiente de atrito
        float getFriction();

        // Aplica uma for�a externa sobre a bola (acumulada)
        void applyForce(glm::vec3& force);

        // Integra o movimento da bola no tempo (baseado em f�sica)
        void integrate(float deltaTime);

        // Verifica e trata colis�es com outra bola
        void handleBallCollision(Ball& otherBall);

        // Verifica e trata colis�es com as paredes do mundo (limites)
        void handleWallCollision(const glm::vec2& minBounds, const glm::vec2& maxBounds);

        // Atualiza toda a f�sica da bola em um frame (tempo delta + colis�es)
        void update(float deltaTime, const glm::vec2& minBounds, const glm::vec2& maxBounds);

    private:
        // ======= Atributos de estado =======

        glm::vec3 position;           // Posi��o atual da bola
        glm::vec3 orientation;        // Rota��o/orienta��o da bola
        RenderProcessing::RenderPro renderPro; // Objeto usado para desenhar a bola

        // ======= Atributos f�sicos =======

        glm::vec3 accumulatedForces = glm::vec3(0.0f);  // Soma de todas as for�as aplicadas (acumulador)
        glm::vec3 velocity = glm::vec3(0.0f);            // Velocidade atual da bola
        float radius = 1.0f;                             // Raio da bola (tamanho)
        float mass = 1.0f;                               // Massa da bola
        float friction = 0.4f;                           // Atrito (op�e-se ao movimento)
        float restitution = 0.8f;                        // Coeficiente de restitui��o (rebote)
    };
}

#endif  // Fim do include guard
