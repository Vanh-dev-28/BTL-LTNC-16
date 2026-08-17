#pragma once
#include <vector>

namespace SpaceInvaders
{
    class Renderer;
    class Bullet;

    enum class CompanionSide
    {
        Left,
        Right
    };

    class CompanionShip
    {
    public:
        CompanionShip(
            CompanionSide side,
            float x,
            float y
        );

        void update(
            float deltaTime,
            float playerX,
            float playerY
        );
        void shoot(std::vector<Bullet>& bullets);
        void render(Renderer& renderer) const;

        bool isActive() const;

        void destroy();

        float getX() const;
        float getY() const;

        float getWidth() const;
        float getHeight() const;
    private:
        CompanionSide side_;
        float shootTimer_;
        float shootCooldown_;

        float x_;
        float y_;

        float width_;
        float height_;

        float speed_;

        float distanceFromPlayer_;

        bool active_;
    };
}