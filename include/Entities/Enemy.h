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
        ZigZag,   // Task B: Zig-zag swarm movement
        Vortex,   // Task D: Vortex swarm movement
        Expansion // Task E: Expansion/Contraction swarm movement
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
        Diving,   // Task C: For dive-bombing
        Returning // Task C: For returning to formation
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

        // --- Getters for GameScene ---
        [[nodiscard]] EnemyState getState() const { return state_; }
        [[nodiscard]] EnemyMovementPattern getMovementPattern() const { return pattern_; }
        [[nodiscard]] const Vector2 &getTargetPosition() const { return targetPosition_; }
        [[nodiscard]] float getSpeed() const { return speed_; }

        // --- Public methods for state changes ---
        void startDive(Vector2 playerPos, EnemyDivePattern pattern);

        // --- Public accessors and mutators ---
        [[nodiscard]] SDL_FRect getRect() const { return {x, y, width, height}; }
        void kill() { alive = false; }

    private:
        float speed_;
        EnemyType type_;
        EnemyMovementPattern pattern_;

        // Entry state
        EnemyState state_;
        EnemyEntryPattern entryPattern_;
        Vector2 startPosition_;
        Vector2 targetPosition_;
        Vector2 controlPoint1_{};
        Vector2 controlPoint2_{};
        float entryProgress_; // 0.0 to 1.0

        // Active state
        float time_;
        float originalY_;

        // --- Dive State Members (Task C) ---
        EnemyDivePattern divePattern_;
        Vector2 diveStartPosition_{};
        Vector2 diveTargetPosition_{};
        float diveProgress_{0.0f};
    };

}