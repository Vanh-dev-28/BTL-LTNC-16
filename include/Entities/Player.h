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
        void update(float deltaTime, std::vector<Bullet> &bullets, bool coneShotActive);
        void render(Renderer &renderer) const;
        void takeDamage(float damage);
        void heal(float amount);

        bool activateFireball(std::vector<Bullet> &bullets);
        bool activateShield();

        [[nodiscard]] bool isAlive() const;
        [[nodiscard]] float getHealth() const;
        [[nodiscard]] float getMaxHealth() const;
        [[nodiscard]] bool isShieldActive() const;
        [[nodiscard]] float getFireballCooldownRatio() const;
        [[nodiscard]] float getShieldCooldownRatio() const;
        [[nodiscard]] float getShieldTimeRatio() const;

    private:
        float speed_;
        float fireCooldown_;
        float health_;
        float maxHealth_;

        bool shieldActive_{false};
        float shieldTimer_{0.0f};
        float shieldCooldown_{0.0f};
        float fireballCooldown_{0.0f};

        void shoot(std::vector<Bullet> &bullets, bool coneShotActive);
    };
}