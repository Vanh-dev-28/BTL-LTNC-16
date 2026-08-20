#pragma once

#include "Core/Renderer.h"

namespace SpaceInvaders
{
    enum class BulletOwner
    {
        Player,
        Enemy,
        Companion
    };

    enum class BulletType
    {
        Normal,
        Fireball
    };

    class Bullet
    {
    public:
        float x, y;
        float width{32.0f};
        float height{32.0f};
        bool active;
        BulletOwner owner;
        BulletType type;

        Bullet(
            float startX,
            float startY,
            float spd,
            BulletOwner owner,
            BulletType type = BulletType::Normal);

        Bullet(
            float startX,
            float startY,
            float velocityX,
            float velocityY,
            BulletOwner owner,
            BulletType type = BulletType::Normal);

        void update(float deltaTime);
        void render(Renderer &renderer) const;
        SDL_FRect getHitbox() const;

    private:
        float velocityX_{0.0f};
        float velocityY_{0.0f};
        void setSize();
    };
}