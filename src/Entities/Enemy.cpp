#include "Entities/Enemy.h"
#include "Managers/TextureManager.h"
#include "Core/Renderer.h"
#include <SDL3/SDL.h>
#include <cmath>

namespace SpaceInvaders
{
    Enemy::Enemy(float startX, float startY, float spd, EnemyType type, EnemyMovementPattern pattern)
        : x(startX), y(startY), width(48.0f), height(48.0f), alive(true), speed_(spd), type_(type), pattern_(pattern), time_(0.0f), originalY_(startY)
    {
        // Randomize start time for sine wave to de-sync enemies
        time_ = (static_cast<float>(rand()) / RAND_MAX) * 10.0f;
    }

    void Enemy::update(float deltaTime, float direction)
    {
        if (!alive)
            return;

        time_ += deltaTime;

        switch (pattern_)
        {
        case EnemyMovementPattern::Horizontal:
            x += direction * speed_ * deltaTime;
            break;
        case EnemyMovementPattern::SineWave:
        {
            x += direction * speed_ * deltaTime;
            // Adjust originalY when the whole swarm moves down
            if (y > originalY_ + 1.0f)
            {
                originalY_ = y;
            }
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