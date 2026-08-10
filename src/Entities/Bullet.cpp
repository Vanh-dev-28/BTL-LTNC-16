#include "Entities/Bullet.h"
#include "Utils/Constants.h"
#include "Managers/TextureManager.h"

namespace SpaceInvaders
{
    Bullet::Bullet(float startX, float startY, float spd, BulletOwner owner)
        : x(startX), y(startY), speed_(spd), active(true), owner(owner) {}

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
        if (!active)
            return;

        if (owner == BulletOwner::Player)
        {
            // Player's bullet is a yellow rectangle
            renderer.fillRect(x, y, 4.0f, 14.0f, SDL_Color{255, 255, 120, 255});
        }
        else // Enemy's bullet
        {
            SDL_Texture *laserTexture = TextureManager::instance().getTexture("enemy_laser");
            if (laserTexture != nullptr)
                renderer.drawTexture(laserTexture, x, y, 9.0f, 17.0f);
            else
                // Fallback to a red rectangle if texture fails to load
                renderer.fillRect(x, y, 4.0f, 16.0f, SDL_Color{255, 40, 40, 255});
        }
    }
}