#include "Entities/Enemy.h"
#include "Managers/TextureManager.h"
#include "Core/Renderer.h"
#include <SDL3/SDL.h>
#include <cmath>
#include "Utils/Vector2.h"

namespace SpaceInvaders
{
    Enemy::Enemy(EnemyType type, EnemyMovementPattern movePattern, EnemyEntryPattern entryPattern, Vector2 startPos, Vector2 targetPos, float speed)
        : x(startPos.x), y(startPos.y), width(48.0f), height(48.0f), alive(true),
          speed_(speed), type_(type), pattern_(movePattern),
          state_(EnemyState::Entering), entryPattern_(entryPattern),
          startPosition_(startPos), targetPosition_(targetPos), entryProgress_(0.0f),
          time_(0.0f), originalY_(targetPos.y)
    {
        // Randomize start time for sine wave to de-sync enemies
        time_ = (static_cast<float>(rand()) / RAND_MAX) * 10.0f;
    }

    // Helper for Quadratic Bezier curve calculation
    Vector2 GetPointOnBezier(Vector2 p0, Vector2 p1, Vector2 p2, float t)
    {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;

        // Tính toán trực tiếp từng tọa độ để tránh thiếu operator* trong struct Vector2
        Vector2 p;
        p.x = uu * p0.x + 2.0f * u * t * p1.x + tt * p2.x;
        p.y = uu * p0.y + 2.0f * u * t * p1.y + tt * p2.y;
        return p;
    }

    void Enemy::update(float deltaTime, float swarmDirection)
    {
        if (!alive)
            return;

        if (state_ == EnemyState::Entering)
        {
            entryProgress_ += deltaTime * 0.75f; // Control entry speed
            if (entryProgress_ > 1.0f)
            {
                entryProgress_ = 1.0f;
            }

            switch (entryPattern_)
            {
            case EnemyEntryPattern::FromTop:
            {
                // Simple Linear Interpolation (Lerp)
                x = startPosition_.x + (targetPosition_.x - startPosition_.x) * entryProgress_;
                y = startPosition_.y + (targetPosition_.y - startPosition_.y) * entryProgress_;
                break;
            }
            case EnemyEntryPattern::ArcFromLeft:
            case EnemyEntryPattern::ArcFromRight:
            {
                // Use a control point to form the arc
                Vector2 controlPoint = {targetPosition_.x, startPosition_.y};
                Vector2 pos = GetPointOnBezier(startPosition_, controlPoint, targetPosition_, entryProgress_);
                x = pos.x;
                y = pos.y;
                break;
            }
            }

            if (entryProgress_ >= 1.0f)
            {
                state_ = EnemyState::Active;
                x = targetPosition_.x;
                y = targetPosition_.y;
                originalY_ = y; // Ensure originalY is correct for sine wave pattern
            }
        }
        else // state_ == EnemyState::Active
        {
            time_ += deltaTime;
            switch (pattern_)
            {
            case EnemyMovementPattern::Horizontal:
                x += swarmDirection * speed_ * deltaTime;
                break;
            case EnemyMovementPattern::SineWave:
                x += swarmDirection * speed_ * deltaTime;
                y = originalY_ + 20.0f * sin(time_ * 2.5f);
                break;
            }
        }
    }

    void Enemy::render(Renderer &renderer) const
    {
        if (!alive)
            return;

        SDL_Texture *enemyTexture = nullptr;
        switch (type_)
        {
        case EnemyType::Bomber:
            enemyTexture = TextureManager::instance().getTexture("enemy_bomber");
            break;
        case EnemyType::Drone:
            enemyTexture = TextureManager::instance().getTexture("enemy_drone");
            break;
        case EnemyType::HealthSpaceship:
            enemyTexture = TextureManager::instance().getTexture("enemy_health_spaceship");
            break;
        }

        if (enemyTexture != nullptr)
        {
            // Sử dụng width và height của instance để render
            renderer.drawTexture(enemyTexture, x, y, width, height);
        }
        else
        {
            // Fallback nếu texture không load được
            renderer.fillRect(x, y, width, height, SDL_Color{255, 80, 80, 255});
        }
    }
}