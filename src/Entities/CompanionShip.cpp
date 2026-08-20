#include "Entities/CompanionShip.h"
#include "Entities/Bullet.h"
#include "Core/Renderer.h"
#include "Managers/TextureManager.h"
#include "Utils/Constants.h"
#include <SDL3/SDL.h>

namespace SpaceInvaders
{
    CompanionShip::CompanionShip(CompanionSide side, float x, float y)
                : side_(side),
                x_(x), y_(y), 
                width_(48.0f), height_(48.0f), 
                speed_(350.0f),
                distanceFromPlayer_(150.0f),
                active_(true),
                shootTimer_(0.0f),
                shootCooldown_(0.8f)
    {
    }

    float CompanionShip::getX() const
    {
        return x_;
    }
    float CompanionShip::getY() const
    {
        return y_;
    }
    float CompanionShip::getWidth() const
    {
        return width_;
    }
    float CompanionShip::getHeight() const
    {
        return height_;
    }

    SDL_FRect CompanionShip::getHitbox() const
    {
        constexpr float hitboxWidth = 26.0f;
        constexpr float hitboxHeight = 34.0f;
        return SDL_FRect{ x_ + (width_ - hitboxWidth) / 2.0f, y_ + (height_ - hitboxHeight) / 2.0f, hitboxWidth, hitboxHeight };
    }

    void CompanionShip::update(float deltaTime, float playerX, float playerY)
    {
        if (!active_)
        {
            return;
        }
        shootTimer_ -= deltaTime;
        if (shootTimer_ < 0.0f)
        {
            shootTimer_ = 0.0f;
        }
        constexpr float screenWidth = Constants::SCREEN_WIDTH;
        constexpr float screenHeight = Constants::SCREEN_HEIGHT;
        float targetX;
        float targetY = playerY;
        if (side_ == CompanionSide::Left)
        {
            targetX = playerX - distanceFromPlayer_;
        }
        else
        {
            targetX = playerX + distanceFromPlayer_;
        }
        if (targetX < 0.0f)
        {
            const float correction = -targetX;
            targetX = 0.0f;
            targetY -= correction;
        }

        if (targetX + width_ > screenWidth)
        {
            const float correction = targetX + width_ - screenWidth;
            targetX = screenWidth - width_;
            targetY -= correction;
        }

        if (targetY < 0.0f)
        {
            targetY = 0.0f;
        }
        if (targetY + height_ > screenHeight)
        {
            targetY = screenHeight - height_;
        }
        const float dx = targetX - x_;
        const float dy = targetY - y_;
        const float distance = SDL_sqrtf(dx * dx + dy * dy);
        if (distance <= 0.0f)
        {
            return;
        }
        const float moveDistance = speed_ * deltaTime;
        if (moveDistance >= distance)
        {
            x_ = targetX;
            y_ = targetY;
        }
        else
        {
            x_ += dx / distance * moveDistance;
            y_ += dy / distance * moveDistance;
        }
    }
    
    void CompanionShip::render(Renderer& renderer) const
    {
        if (!active_)
        {
            return;
        }
        SDL_Texture* texture = TextureManager::instance().getTexture("companion_ship");
        if (texture == nullptr)
        {
            return;
        }

        renderer.drawTexture(texture, x_, y_, width_, height_);
    }
    bool CompanionShip::isActive() const
    {
        return active_;
    }

    void CompanionShip::destroy()
    {
        active_ = false;
    }
    void CompanionShip::shoot(std::vector<Bullet>& bullets)
    {
        if (!active_)
        {
            return;
        }

        if (shootTimer_ > 0.0f)
        {
            return;
        }
        const float bulletWidth = 8.0f;
        const float bulletHeight = 20.0f;
        const float bulletX = x_ + width_ / 2.0f - bulletWidth / 2.0f;
        const float bulletY = y_ - bulletHeight;
        bullets.emplace_back(bulletX, bulletY, 0.0f, -500.0f, BulletOwner::Player);
        Bullet& bullet = bullets.back();
        bullet.width = bulletWidth;
        bullet.height = bulletHeight;
        shootTimer_ = shootCooldown_;
    }
}