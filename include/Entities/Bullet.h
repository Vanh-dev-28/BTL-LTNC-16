#pragma once

#include "Core/Renderer.h"

namespace SpaceInvaders
{
    class Bullet
    {
    public:
        float x, y;
        bool active;

        Bullet(float startX, float startY, float spd);

        void update(float deltaTime);
        void render(Renderer &renderer) const;

    private:
        float speed_;
    };
}