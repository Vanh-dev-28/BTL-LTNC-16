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
        float targetX = playerX;

        if (side_ == CompanionSide::Left)
        {   
            targetX -= distanceFromPlayer_;
        }
        else
        {
            targetX += distanceFromPlayer_;
        }
        float targetY = playerY;
        float dx = targetX - x_;
        float dy = targetY - y_;
        float distance = SDL_sqrtf(dx * dx + dy * dy);
        if (distance > 0.0f)
        {
            float moveDistance = speed_ * deltaTime;
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

    const float bulletX =
        x_ + width_ / 2.0f - bulletWidth / 2.0f;

    const float bulletY =
        y_ - bulletHeight;

    bullets.emplace_back(
        bulletX,
        bulletY,
        0.0f,       // velocity X
        -500.0f,    // velocity Y → bay LÊN
        BulletOwner::Player
    );

    Bullet& bullet = bullets.back();

    bullet.width = bulletWidth;
    bullet.height = bulletHeight;

    shootTimer_ = shootCooldown_;
    }
}