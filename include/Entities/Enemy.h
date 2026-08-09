#pragma once

#include "Core/Renderer.h"

namespace SpaceInvaders
{

    enum class EnemyType
    {
        Bomber,
        Drone,
        HealthSpaceship
    };

    class Enemy
    {
    public:
        float x, y;
        float width, height;
        bool alive;

        Enemy(float startX, float startY, float spd, EnemyType type);

        void update(float deltaTime, float direction);
        void render(Renderer &renderer) const;

    private:
        float speed_;
        EnemyType type_;
    };

}