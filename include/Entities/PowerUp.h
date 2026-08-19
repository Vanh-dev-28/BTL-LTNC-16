#pragma once

#include "Utils/Vector2.h"

namespace SpaceInvaders
{
    class Renderer;

    enum class PowerUpType
    {
        ConeShot, 
        Heal,
        Companion
    };
    class PowerUp
    {
    public:
        PowerUp(PowerUpType type, float x, float y);
        void update(float deltaTime);
        void render(Renderer& renderer) const;
        bool isActive() const;
        void collect();
        bool isCollectedBy(float playerX, float playerY, float playerWidth, float playerHeight) const;
        PowerUpType getType() const;
        float getX() const;
        float getY() const;
    private:
        PowerUpType type_;
        float x_;
        float y_;
        float width_;
        float height_;
        float speed_;
        bool active_;
    };
}