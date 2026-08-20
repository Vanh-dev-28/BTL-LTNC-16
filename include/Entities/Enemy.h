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
        SineWave,
        ZigZag,
        Vortex,
        Expansion
    };

    enum class EnemyEntryPattern
    {
        FromTop,
        ArcFromLeft,
        ArcFromRight,
        Galaga
    };

    enum class EnemyState
    {
        Entering,
        Active,
        Diving,
        Returning
    };

    enum class EnemyDivePattern
    {
        Straight,
        Curved
    };

    class Enemy
    {
    public:
        float x, y;
        float width, height;
        bool alive;

        Enemy(EnemyType type, EnemyMovementPattern movePattern, EnemyEntryPattern entryPattern, Vector2 startPos, Vector2 targetPos, float speed);
        Enemy(EnemyType type, EnemyMovementPattern movePattern, EnemyEntryPattern entryPattern, Vector2 startPos, Vector2 targetPos, float speed, Vector2 control1, Vector2 control2);

        void update(float deltaTime, Vector2 swarmVelocity);
        void render(Renderer &renderer) const;

        [[nodiscard]] EnemyState getState() const { return state_; }
        [[nodiscard]] EnemyMovementPattern getMovementPattern() const { return pattern_; }
        [[nodiscard]] const Vector2 &getTargetPosition() const { return targetPosition_; }
        [[nodiscard]] float getSpeed() const { return speed_; }

        void startDive(Vector2 playerPos, EnemyDivePattern pattern);

        [[nodiscard]] SDL_FRect getRect() const { return {x, y, width, height}; }
        void kill() { alive = false; }

        void takeDamage(float damage);
        [[nodiscard]] float getHealth() const { return health_; }
        [[nodiscard]] float getMaxHealth() const { return maxHealth_; }
        [[nodiscard]] EnemyType getType() const { return type_; }

    private:
        float speed_;
        EnemyType type_;
        EnemyMovementPattern pattern_;

        EnemyState state_;
        EnemyEntryPattern entryPattern_;
        Vector2 startPosition_;
        Vector2 targetPosition_;
        Vector2 controlPoint1_{};
        Vector2 controlPoint2_{};
        float entryProgress_; // 0.0 to 1.0

        float time_;
        float originalY_;

        EnemyDivePattern divePattern_;
        Vector2 diveStartPosition_{};
        Vector2 diveTargetPosition_{};
        float diveProgress_{0.0f};

        float health_;
        float maxHealth_;
    };

}