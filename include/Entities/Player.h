#pragma once

#include <vector>

// Forward declarations để tránh include vòng lặp và giảm thời gian biên dịch
namespace SpaceInvaders
{
    class Renderer;
    class Bullet;
}

namespace SpaceInvaders
{
    class Player
    {
    public:
        float x, y;

        Player();

        void init();
        void update(float deltaTime, std::vector<Bullet> &bullets);
        void render(Renderer &renderer) const;

    private:
        float speed_;
        float fireCooldown_;

        void shoot(std::vector<Bullet> &bullets);
    };
}