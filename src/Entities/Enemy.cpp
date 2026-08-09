#include "Entities/Enemy.h"
#include "Managers/TextureManager.h"
#include "Core/Renderer.h"
#include <SDL3/SDL.h>

namespace SpaceInvaders
{
    Enemy::Enemy(float startX, float startY, float spd, EnemyType type)
        : x(startX), y(startY), width(48.0f), height(48.0f), alive(true), speed_(spd), type_(type)
    {
        // Có thể điều chỉnh kích thước theo loại enemy nếu cần
        // Ví dụ: if (type == EnemyType::Bomber) { width = 52.0f; }
    }

    void Enemy::update(float deltaTime, float direction)
    {
        if (!alive)
            return;
        x += direction * speed_ * deltaTime;
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