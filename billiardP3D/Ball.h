#ifndef BALL_H
#define BALL_H  

#include <glm/glm.hpp>              
#include "RenderProcessing.h"       

namespace Ball
{
    // Definição da classe Ball (bola)
    class Ball
    {
    public:
        // Construtor e destrutor
        Ball();     // Inicializa valores padrão
        ~Ball();    // Libera recursos (se necessário)

        // Define a posição da bola no espaço 3D
        void setPosition(glm::vec3 position);

        // Define a orientação (rotacionamento) da bola
        void setOrientation(glm::vec3 rotation);

        // Obtém a posição atual
        glm::vec3 getPosition();

        // Obtém a orientação atual
        glm::vec3 getOrientation();

        // Define o objeto de renderização da bola
        void setRenderPro(RenderProcessing::RenderPro renderPro);

        // Copia os dados de renderização para outro objeto
        void getRenderPro(RenderProcessing::RenderPro& renderPro);

        // ======= Funções relacionadas à física =======

        // Define a velocidade da bola
        void setVelocity(glm::vec3 velocity);

        // Obtém a velocidade atual
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

        // Aplica uma força externa sobre a bola (acumulada)
        void applyForce(glm::vec3& force);

        // Integra o movimento da bola no tempo (baseado em física)
        void integrate(float deltaTime);

        // Verifica e trata colisões com outra bola
        void handleBallCollision(Ball& otherBall);

        // Verifica e trata colisões com as paredes do mundo (limites)
        void handleWallCollision(const glm::vec2& minBounds, const glm::vec2& maxBounds);

        // Atualiza toda a física da bola em um frame (tempo delta + colisões)
        void update(float deltaTime, const glm::vec2& minBounds, const glm::vec2& maxBounds);

    private:
        // ======= Atributos de estado =======

        glm::vec3 position;           // Posição atual da bola
        glm::vec3 orientation;        // Rotação/orientação da bola
        RenderProcessing::RenderPro renderPro; // Objeto usado para desenhar a bola

        // ======= Atributos físicos =======

        glm::vec3 accumulatedForces = glm::vec3(0.0f);  // Soma de todas as forças aplicadas (acumulador)
        glm::vec3 velocity = glm::vec3(0.0f);            // Velocidade atual da bola
        float radius = 1.0f;                             // Raio da bola (tamanho)
        float mass = 1.0f;                               // Massa da bola
        float friction = 0.4f;                           // Atrito (opõe-se ao movimento)
        float restitution = 0.8f;                        // Coeficiente de restituição (rebote)
    };
}

#endif  // Fim do include guard
