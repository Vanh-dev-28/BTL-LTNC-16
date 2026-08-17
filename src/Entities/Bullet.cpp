#include "Entities/Bullet.h"
#include "Utils/Constants.h"
#include "Managers/TextureManager.h"

namespace SpaceInvaders
{
    Bullet::Bullet(
        float startX,
        float startY,
        float spd,
        BulletOwner bulletOwner,
        BulletType bulletType)
        : x(startX),
          y(startY),
          active(true),
          owner(bulletOwner),
          type(bulletType),
          velocityX_(0.0f),
          velocityY_(spd)
    {
        setSize();
    }

    Bullet::Bullet(
        float startX,
        float startY,
        float velocityX,
        float velocityY,
        BulletOwner bulletOwner,
        BulletType bulletType)
        : x(startX),
          y(startY),
          active(true),
          owner(bulletOwner),
          type(bulletType),
          velocityX_(velocityX),
          velocityY_(velocityY)
    {
        setSize();
    }

    void Bullet::setSize()
    {
        // Phân loại kích thước chuẩn cho từng loại đạn
        if (owner == BulletOwner::Player)
        {
            if (type == BulletType::Fireball)
            {
                width = 32.0f;
                height = 32.0f;
            }
            else // Đạn thường của người chơi
            {
                width = 6.0f;
                height = 18.0f;
            }
        }
        else // BulletOwner::Enemy (Laser của quái - to và rõ ràng hơn)
        {
            width = 50.0f;   // 10.0f * 5
            height = 125.0f; // 25.0f * 5
        }
    }

    void Bullet::update(float deltaTime)
    {
        if (!active)
            return;

        x += velocityX_ * deltaTime;
        y += velocityY_ * deltaTime;

        if (x < -50.0f ||
            x > Constants::SCREEN_WIDTH + 50.0f ||
            y < -50.0f ||
            y > Constants::SCREEN_HEIGHT + 50.0f)
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
                SDL_Texture *fireballTexture =
                    TextureManager::instance().getTexture("fireball_effect");

                if (fireballTexture != nullptr)
                {
                    renderer.drawTexture(
                        fireballTexture,
                        x,
                        y,
                        width,
                        height);
                }
                else
                {
                    renderer.fillRect(
                        x,
                        y,
                        width,
                        height,
                        SDL_Color{0, 255, 255, 255});
                }
            }
            else // Đạn thường Player
            {
                renderer.fillRect(
                    x,
                    y,
                    width,
                    height,
                    SDL_Color{255, 255, 120, 255});
            }
        }
        else // BulletOwner::Enemy (Laser đạn quái)
        {
            SDL_Texture *laserTexture =
                TextureManager::instance().getTexture("enemy_laser");

            if (laserTexture != nullptr)
            {
                renderer.drawTexture(
                    laserTexture,
                    x,
                    y,
                    width,
                    height);
            }
            else
            {
                renderer.fillRect(
                    x,
                    y,
                    width,
                    height,
                    SDL_Color{255, 40, 40, 255});
            }
        }
    }

} // namespace SpaceInvaders