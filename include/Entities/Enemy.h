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

    enum class EnemyMovementPattern
    {
        Horizontal,
        SineWave
    };

    class Enemy
    {
    public:
        float x, y;
        float width, height;
        bool alive;

        Enemy(float startX, float startY, float spd, EnemyType type, EnemyMovementPattern pattern);

        void update(float deltaTime, float direction);
        void render(Renderer &renderer) const;

    private:
        float speed_;
        EnemyType type_;
        EnemyMovementPattern pattern_;

        // State for movement patterns
        float time_;
        float originalY_;
    };

}