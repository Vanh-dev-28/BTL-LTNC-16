#include "Entities/Enemy.h"
#include "Managers/TextureManager.h"
#include "Core/Renderer.h"
#include "Utils/Constants.h"
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
          time_(0.0f), originalY_(targetPos.y), health_(1.0f), maxHealth_(1.0f)
    {
        switch (type_)
        {
            case EnemyType::Bomber:
                maxHealth_ = 4.0f;
                break;
            case EnemyType::Drone:
                maxHealth_ = 2.0f;
                break;
            case EnemyType::HealthSpaceship:
                maxHealth_ = 6.0f;
                break;
        }

        health_ = maxHealth_;
        time_ = (static_cast<float>(rand()) / RAND_MAX) * 10.0f;
    }

    // Constructor for cubic Bezier (Galaga-style)
    Enemy::Enemy(EnemyType type, EnemyMovementPattern movePattern, EnemyEntryPattern entryPattern, Vector2 startPos, Vector2 targetPos, float speed, Vector2 control1, Vector2 control2)
        : x(startPos.x), y(startPos.y), width(48.0f), height(48.0f), alive(true),
          speed_(speed), type_(type), pattern_(movePattern),
          state_(EnemyState::Entering), entryPattern_(entryPattern),
          startPosition_(startPos), targetPosition_(targetPos), controlPoint1_(control1), controlPoint2_(control2), entryProgress_(0.0f),
          time_(0.0f), originalY_(targetPos.y), health_(1.0f), maxHealth_(1.0f)
    {
        switch (type_)
        {
            case EnemyType::Bomber:
                maxHealth_ = 4.0f;
                break;
            case EnemyType::Drone:
                maxHealth_ = 2.0f;
                break;
            case EnemyType::HealthSpaceship:
                maxHealth_ = 6.0f;
                break;
        }

        health_ = maxHealth_;
        time_ = (static_cast<float>(rand()) / RAND_MAX) * 10.0f;
    }

    void Enemy::takeDamage(float damage)
    {
        if (!alive)
            return;

        health_ -= damage;

        if (health_ <= 0.0f)
        {
            health_ = 0.0f;
            alive = false;
        }
    }

    // Helper for Quadratic Bezier curve calculation
    Vector2 GetPointOnBezier(Vector2 p0, Vector2 p1, Vector2 p2, float t)
    {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;

        Vector2 p;
        p.x = uu * p0.x + 2.0f * u * t * p1.x + tt * p2.x;
        p.y = uu * p0.y + 2.0f * u * t * p1.y + tt * p2.y;
        return p;
    }

    // Helper for Cubic Bezier curve calculation
    Vector2 GetPointOnCubicBezier(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t)
    {
        float u = 1.0f - t;
        float tt = t * t;
        float ttt = tt * t;
        float uu = u * u;
        float uuu = uu * u;

        Vector2 p;
        p.x = uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x;
        p.y = uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y;
        return p;
    }

    void Enemy::startDive(Vector2 playerPos, EnemyDivePattern pattern)
    {
        if (state_ == EnemyState::Active)
        {
            state_ = EnemyState::Diving;
            divePattern_ = pattern;
            diveStartPosition_ = {x, y};
            diveTargetPosition_ = playerPos;
            diveProgress_ = 0.0f;
        }
    }
    void Enemy::update(float deltaTime, Vector2 swarmVelocity)
    {
        if (!alive)
            return;

        if (state_ == EnemyState::Entering)
        {
            entryProgress_ += deltaTime * 0.75f;
            if (entryProgress_ > 1.0f)
            {
                entryProgress_ = 1.0f;
            }

            switch (entryPattern_)
            {
            case EnemyEntryPattern::FromTop:
            {
                float x_offset = (startPosition_.x < Constants::SCREEN_WIDTH / 2.0f) ? 80.0f : -80.0f;
                Vector2 controlPoint = {startPosition_.x + x_offset, startPosition_.y + (targetPosition_.y - startPosition_.y) * 0.4f};
                Vector2 pos = GetPointOnBezier(startPosition_, controlPoint, targetPosition_, entryProgress_);
                x = pos.x;
                y = pos.y;
                break;
            }
            case EnemyEntryPattern::ArcFromLeft:
            case EnemyEntryPattern::ArcFromRight:
            {
                Vector2 controlPoint = {targetPosition_.x, startPosition_.y};
                Vector2 pos = GetPointOnBezier(startPosition_, controlPoint, targetPosition_, entryProgress_);
                x = pos.x;
                y = pos.y;
                break;
            }
            case EnemyEntryPattern::Galaga:
            {
                Vector2 pos = GetPointOnCubicBezier(startPosition_, controlPoint1_, controlPoint2_, targetPosition_, entryProgress_);
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
                originalY_ = y;
            }
        }
        else if (state_ == EnemyState::Diving)
        {
            diveProgress_ += deltaTime * 0.5f; // Tốc độ bổ nhào

            if (divePattern_ == EnemyDivePattern::Curved)
            {
                // Đường cong Bezier bậc 2 hướng về người chơi
                Vector2 control = {
                    (diveStartPosition_.x + diveTargetPosition_.x) / 2.0f,
                    diveStartPosition_.y + 150.0f};
                Vector2 pos = GetPointOnBezier(diveStartPosition_, control, diveTargetPosition_, diveProgress_);
                x = pos.x;
                y = pos.y;
            }
            else // Bổ nhào đường thẳng (Straight dive)
            {
                x = diveStartPosition_.x + (diveTargetPosition_.x - diveStartPosition_.x) * diveProgress_;
                y = diveStartPosition_.y + (diveTargetPosition_.y - diveStartPosition_.y) * diveProgress_;
            }

            // Khi quái bay quá đáy màn hình hoặc hoàn thành bổ nhào
            if (y > Constants::SCREEN_HEIGHT + 50.0f || diveProgress_ >= 1.2f)
            {
                state_ = EnemyState::Returning;

                // Đưa quái xuất hiện lại ở trên đỉnh màn hình (chuẩn phong cách Galaga)
                // thay vì bay lùi ngược từ dưới đáy lên
                diveStartPosition_ = {targetPosition_.x, -50.0f};
                x = diveStartPosition_.x;
                y = diveStartPosition_.y;
                diveProgress_ = 0.0f;
            }
        }
        else if (state_ == EnemyState::Returning)
        {
            diveProgress_ += deltaTime * 1.0f; // Tốc độ quay về vị trí đội hình
            if (diveProgress_ > 1.0f)
            {
                diveProgress_ = 1.0f;
            }

            // Nội suy mượt mà từ trên đỉnh màn hình đáp xuống vị trí đội hình (targetPosition_)
            x = diveStartPosition_.x + (targetPosition_.x - diveStartPosition_.x) * diveProgress_;
            y = diveStartPosition_.y + (targetPosition_.y - diveStartPosition_.y) * diveProgress_;

            if (diveProgress_ >= 1.0f)
            {
                state_ = EnemyState::Active;
                x = targetPosition_.x;
                y = targetPosition_.y;
                originalY_ = y;
            }
        }
        else // state_ == EnemyState::Active
        {
            time_ += deltaTime;
            x += swarmVelocity.x * deltaTime;
            y += swarmVelocity.y * deltaTime;

            switch (pattern_)
            {
            case EnemyMovementPattern::SineWave:
                y = originalY_ + 20.0f * sin(time_ * 2.5f);
                break;
            default:
                // Các kiểu di chuyển khác (Horizontal, ZigZag, Vortex, Expansion)
                // đã được xử lý qua swarmVelocity
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
            renderer.drawTexture(enemyTexture, x, y, width, height);
        }
        else
        {
            renderer.fillRect(x, y, width, height, SDL_Color{255, 80, 80, 255});
        }
    }
}