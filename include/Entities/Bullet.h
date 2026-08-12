#pragma once

#include "Core/Renderer.h"

namespace SpaceInvaders
{
    enum class BulletOwner
    {
        Player,
        Enemy
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
        float width{4.0f};
        float height{12.0f};
        bool active;
        BulletOwner owner;
        BulletType type;

        Bullet(float startX, float startY, float spd, BulletOwner owner, BulletType type = BulletType::Normal);

        void update(float deltaTime);
        void render(Renderer &renderer) const;

    private:
        float speed_;
    };
}