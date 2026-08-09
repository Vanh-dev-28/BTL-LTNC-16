#include "Entities/Bullet.h"
#include "Utils/Constants.h"

namespace SpaceInvaders
{
    Bullet::Bullet(float startX, float startY, float spd)
        : x(startX), y(startY), speed_(spd), active(true) {}

    void Bullet::update(float deltaTime)
    {
        if (!active)
            return;
        y += speed_ * deltaTime;
        if (y < -20.0f || y > Constants::SCREEN_HEIGHT + 20.0f)
        {
            active = false;
        }
    }

    void Bullet::render(Renderer &renderer) const
    {
        if (active)
        {
            renderer.fillRect(x, y, 4.0f, 14.0f, SDL_Color{255, 255, 120, 255});
        }
    }
}