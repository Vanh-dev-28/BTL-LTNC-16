#include "Entities/Bullet.h"
#include "Utils/Constants.h"
#include "Managers/TextureManager.h"

namespace SpaceInvaders
{
    Bullet::Bullet(float startX, float startY, float spd, BulletOwner owner, BulletType type)
        : x(startX), y(startY), speed_(spd), active(true), owner(owner), type(type) {}

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
            if (type == BulletType::Fireball)
            {
                SDL_Texture *fireballTexture = TextureManager::instance().getTexture("fireball_effect");
                if (fireballTexture)
                    renderer.drawTexture(fireballTexture, x, y, width, height);
                else // Fallback
                    renderer.fillRect(x, y, width, height, SDL_Color{0, 255, 255, 255});
            }
            else // Normal player bullet
            {
                renderer.fillRect(x, y, width, height, SDL_Color{255, 255, 120, 255});
            }
        }
        else // Enemy's bullet (always normal type for now)
        {
            SDL_Texture *laserTexture = TextureManager::instance().getTexture("enemy_laser");
            if (laserTexture != nullptr)
                renderer.drawTexture(laserTexture, x, y, width, height);
            else
                // Fallback to a red rectangle if texture fails to load
                renderer.fillRect(x, y, width, height, SDL_Color{255, 40, 40, 255});
        }
    }
}