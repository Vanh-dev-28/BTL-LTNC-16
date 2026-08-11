#pragma once

#include "Core/Renderer.h"
#include "Utils/Vector2.h"

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

    enum class EnemyEntryPattern
    {
        FromTop,
        ArcFromLeft,
        ArcFromRight
    };

    enum class EnemyState
    {
        Entering,
        Active
    };

    class Enemy
    {
    public:
        float x, y;
        float width, height;
        bool alive;

        Enemy(EnemyType type, EnemyMovementPattern movePattern, EnemyEntryPattern entryPattern, Vector2 startPos, Vector2 targetPos, float speed);

        void update(float deltaTime, float swarmDirection);
        void render(Renderer &renderer) const;

    private:
        float speed_;
        EnemyType type_;
        EnemyMovementPattern pattern_;

        // Entry state
        EnemyState state_;
        EnemyEntryPattern entryPattern_;
        Vector2 startPosition_;
        Vector2 targetPosition_;
        float entryProgress_; // 0.0 to 1.0

        // Active state
        float time_;
        float originalY_;
    };

}