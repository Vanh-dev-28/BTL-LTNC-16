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
        void takeDamage(float damage);

        [[nodiscard]] bool isAlive() const;
        [[nodiscard]] float getHealth() const;
        [[nodiscard]] float getMaxHealth() const;

    private:
        float speed_;
        float fireCooldown_;
        float health_;
        float maxHealth_;

        void shoot(std::vector<Bullet> &bullets);
    };
}