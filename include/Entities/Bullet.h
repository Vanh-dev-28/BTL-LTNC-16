#pragma once

#include "Core/Renderer.h"

namespace SpaceInvaders
{
    enum class BulletOwner
    {
        Player,
        Enemy
    };

    class Bullet
    {
    public:
        float x, y;
        bool active;
        BulletOwner owner;

        Bullet(float startX, float startY, float spd, BulletOwner owner);

        void update(float deltaTime);
        void render(Renderer &renderer) const;

    private:
        float speed_;
    };
}