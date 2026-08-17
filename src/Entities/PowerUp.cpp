#include "Entities/PowerUp.h"
#include "Core/Renderer.h"
#include "Managers/TextureManager.h"
#include "Utils/Constants.h"

namespace SpaceInvaders
{

PowerUp::PowerUp(
    PowerUpType type,
    float x,
    float y
)
    : type_(type),
      x_(x),
      y_(y),
      width_(48.0f),
      height_(48.0f),
      speed_(100.0f),
      active_(true)
{
}

void PowerUp::update(float deltaTime)
{
    if (!active_)
        return;
    y_ += speed_ * deltaTime;

    if (y_ > Constants::SCREEN_HEIGHT + height_)
    {
        active_ = false;
    }
}

void PowerUp::render(Renderer& renderer) const
{
    if (!active_)
        return;

    SDL_Texture* texture = nullptr;

    switch (type_)
    {
    case PowerUpType::ConeShot:
        texture = TextureManager::instance().getTexture("powerup_cone_shot");
        break;
    case PowerUpType::Heal:
        texture = TextureManager::instance().getTexture("powerup_heal");
        break;
    case PowerUpType::Companion:
        texture = TextureManager::instance().getTexture("powerup_companion");
        break;
    }

    if (texture != nullptr)
    {
        renderer.drawTexture(
            texture,
            x_,
            y_,
            width_,
            height_
        );
    }
    else
    {
        // Fallback nếu texture chưa load được
        renderer.fillRect(
            x_,
            y_,
            width_,
            height_,
            SDL_Color{255, 165, 0, 255}
        );
    }
}

bool PowerUp::isActive() const
{
    return active_;
}

void PowerUp::collect()
{
    active_ = false;
}

bool PowerUp::isCollectedBy(
    float playerX,
    float playerY,
    float playerWidth,
    float playerHeight
) const
{
    if (!active_)
        return false;

    return
        x_ < playerX + playerWidth &&
        x_ + width_ > playerX &&
        y_ < playerY + playerHeight &&
        y_ + height_ > playerY;
}

PowerUpType PowerUp::getType() const
{
    return type_;
}

float PowerUp::getX() const
{
    return x_;
}

float PowerUp::getY() const
{
    return y_;
}

}