#include "Entities/Player.h"
#include "Entities/Bullet.h"
#include "Core/Renderer.h"
#include "Managers/TextureManager.h"
#include "Utils/Constants.h"
#include <SDL3/SDL.h>
#include <cmath>

namespace SpaceInvaders
{
    Player::Player() : x(0), y(0), speed_(0), fireCooldown_(0.18f), health_(Constants::PLAYER_MAX_HEALTH), maxHealth_(Constants::PLAYER_MAX_HEALTH) {}

    void Player::init()
    {
        x = Constants::SCREEN_WIDTH / 2.0f - 32.0f;
        y = Constants::SCREEN_HEIGHT - 90.0f;
        speed_ = 360.0f;
        fireCooldown_ = 0.0f;
        health_ = maxHealth_;

        shieldActive_ = false;
        shieldTimer_ = 0.0f;
        shieldCooldown_ = 0.0f;
        fireballCooldown_ = 0.0f;
    }

    void Player::update(float deltaTime, std::vector<Bullet> &bullets, bool coneShotActive)
    {
        // --- Mouse Movement Logic ---
        float mouseScreenX, mouseScreenY;
        SDL_GetMouseState(&mouseScreenX, &mouseScreenY);

        // Convert mouse screen coordinates to game world coordinates
        // The player sprite's center will follow the cursor
        float targetX = (mouseScreenX - Renderer::s_offsetX) / Renderer::s_scale;
        float targetY = (mouseScreenY - Renderer::s_offsetY) / Renderer::s_scale;

        // Adjust target to center the player sprite on the cursor
        const float playerSpriteWidth = 48.0f;
        const float playerSpriteHeight = 48.0f;
        targetX -= playerSpriteWidth / 2.0f;
        targetY -= playerSpriteHeight / 2.0f;

        // Smoothly move the player towards the target X position
        const float followSpeed = 7.0f;
        x += (targetX - x) * followSpeed * deltaTime;
        y += (targetY - y) * followSpeed * deltaTime;

        // --- Boundary Clamping ---
        const float margin = 20.0f; // 20 pixels from all edges
        if (x < margin)
            x = margin;
        if (x > Constants::SCREEN_WIDTH - playerSpriteWidth - margin)
            x = Constants::SCREEN_WIDTH - playerSpriteWidth - margin;

        if (y < margin)
            y = margin;
        if (y > Constants::SCREEN_HEIGHT - playerSpriteHeight - margin)
            y = Constants::SCREEN_HEIGHT - playerSpriteHeight - margin;

        // --- Shooting Logic (Unchanged) ---
        const bool *keyboardState = SDL_GetKeyboardState(nullptr);
        fireCooldown_ -= deltaTime;
        if (fireCooldown_ <= 0.0f && (keyboardState[SDL_SCANCODE_SPACE] || keyboardState[SDL_SCANCODE_J]))
        {
            shoot(bullets, coneShotActive);
            fireCooldown_ = 0.18f;
        }

        // --- Ability Timers ---
        if (fireballCooldown_ > 0.0f)
        {
            fireballCooldown_ -= deltaTime;
        }
        if (shieldCooldown_ > 0.0f)
        {
            shieldCooldown_ -= deltaTime;
        }

        if (shieldActive_)
        {
            shieldTimer_ -= deltaTime;
            if (shieldTimer_ <= 0.0f)
            {
                shieldActive_ = false;
            }
        }
    }

    void Player::render(Renderer &renderer) const
    {
        if (!isAlive())
        {
            return;
        }

        // 1. Vẽ hiệu ứng khiên (shield) TRƯỚC
        if (isShieldActive())
        {
            SDL_Texture *shieldTexture = TextureManager::instance().getTexture("shield_effect");
            if (shieldTexture)
            {
                const float shieldSize = 80.0f;
                const float playerSize = 48.0f;
                const float shieldX = x + (playerSize - shieldSize) / 2.0f;
                const float shieldY = y + (playerSize - shieldSize) / 2.0f;

                // Thêm hiệu ứng nhấp nháy cho khiên thêm sinh động
                // Alpha sẽ thay đổi từ (150-105) đến (150+105) -> 45 đến 255
                float alpha = 150.0f + 105.0f * sin(SDL_GetTicks() / 200.0f);
                SDL_SetTextureAlphaMod(shieldTexture, static_cast<Uint8>(alpha));

                renderer.drawTexture(shieldTexture, shieldX, shieldY, shieldSize, shieldSize);

                // Reset alpha để không ảnh hưởng đến các đối tượng khác
                SDL_SetTextureAlphaMod(shieldTexture, 255);
            }
        }

        // 2. Vẽ tàu của người chơi SAU (đè lên trên khiên)
        SDL_Texture *shipTexture = TextureManager::instance().getTexture("ship");
        if (shipTexture)
        {
            renderer.drawTexture(shipTexture, x, y, 48.0f, 48.0f);
        }
        else
        {
            renderer.fillRect(x, y, 52.0f, 24.0f, SDL_Color{255, 255, 255, 255});
            renderer.fillRect(x + 16.0f, y - 12.0f, 20.0f, 16.0f, SDL_Color{255, 255, 255, 255});
        }
    }

    void Player::shoot(std::vector<Bullet> &bullets, bool coneShotActive)
    {
        const float bulletX = x + 22.0f;
        const float bulletY = y - 14.0f;

        if (!coneShotActive)
        {
            bullets.emplace_back(bulletX, bulletY, -420.0f, BulletOwner::Player);
            return;
        }

        bullets.emplace_back(bulletX, bulletY, -170.0f, -420.0f, BulletOwner::Player);

        bullets.emplace_back(bulletX, bulletY, 0.0f, -500.0f, BulletOwner::Player);

        bullets.emplace_back(bulletX, bulletY, 170.0f, -420.0f, BulletOwner::Player);
    }

    bool Player::activateFireball(std::vector<Bullet> &bullets)
    {
        if (fireballCooldown_ > 0.0f)
        {
            return false;
        }
        Bullet &fireball = bullets.emplace_back(0.0f, 0.0f, -800.0f, BulletOwner::Player, BulletType::Fireball);
        fireball.width = 40.0f;
        fireball.height = 40.0f;
        fireball.x = x + (48.0f / 2.0f) - (fireball.width / 2.0f);
        fireball.y = y;           // Start at player's y
        fireballCooldown_ = 4.0f; // 4 second cooldown
        return true;
    }

    bool Player::activateShield()
    {
        if (shieldCooldown_ > 0.0f || shieldActive_)
        {
            return false;
        }
        if (shieldCooldown_ <= 0.0f)
        {
            shieldActive_ = true;
            shieldTimer_ = 5.0f;     // 5 second duration
            shieldCooldown_ = 10.0f; // 10 second cooldown
        }
        return true;
    }

    void Player::takeDamage(float damage)
    {
        health_ -= damage;
        if (health_ < 0)
        {
            health_ = 0;
        }
    }

    bool Player::isAlive() const
    {
        return health_ > 0;
    }

    float Player::getHealth() const
    {
        return health_;
    }

    float Player::getMaxHealth() const
    {
        return maxHealth_;
    }

    bool Player::isShieldActive() const
    {
        return shieldActive_;
    }

    float Player::getFireballCooldownRatio() const
    {
        if (8.0f <= 0)
            return 0.0f;
        return std::max(0.0f, fireballCooldown_ / 8.0f);
    }

    float Player::getShieldCooldownRatio() const
    {
        if (15.0f <= 0)
            return 0.0f;
        return std::max(0.0f, shieldCooldown_ / 15.0f);
    }

    float Player::getShieldTimeRatio() const
    {
        if (!shieldActive_ || 5.0f <= 0)
            return 0.0f;
        return std::max(0.0f, shieldTimer_ / 5.0f);
    }

    void Player::heal(float amount)
    {
        health_ += amount;

        if (health_ > maxHealth_)
        {
            health_ = maxHealth_;
        }
    }
}