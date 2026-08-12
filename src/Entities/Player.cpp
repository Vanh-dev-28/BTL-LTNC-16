#include "Entities/Player.h"
#include "Entities/Bullet.h"
#include "Core/Renderer.h"
#include "Managers/TextureManager.h"
#include "Utils/Constants.h"
#include <SDL3/SDL.h>

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

    void Player::update(float deltaTime, std::vector<Bullet> &bullets)
    {
        // --- Mouse Movement Logic ---
        float mouseScreenX;
        SDL_GetMouseState(&mouseScreenX, nullptr);

        // Convert mouse screen coordinates to game world coordinates
        // The player sprite's center will follow the cursor
        float targetX = (mouseScreenX - Renderer::s_offsetX) / Renderer::s_scale;
        targetX -= 48.0f / 2.0f; // Player width is 48.0f

        // Smoothly move the player towards the target X position
        const float followSpeed = 7.0f;
        x += (targetX - x) * followSpeed * deltaTime;

        // --- Boundary Clamping ---
        if (x < 20.0f)
            x = 20.0f;
        if (x > Constants::SCREEN_WIDTH - 64.0f) // Preserving original boundary logic
            x = Constants::SCREEN_WIDTH - 64.0f;

        // --- Shooting Logic (Unchanged) ---
        const bool *keyboardState = SDL_GetKeyboardState(nullptr);
        fireCooldown_ -= deltaTime;
        if (fireCooldown_ <= 0.0f && (keyboardState[SDL_SCANCODE_SPACE] || keyboardState[SDL_SCANCODE_J]))
        {
            shoot(bullets);
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
        SDL_Texture *playerTexture = TextureManager::instance().getTexture("ship");
        if (playerTexture != nullptr)
        {
            renderer.drawTexture(playerTexture, x, y, 48.0f, 48.0f);
        }
        else
        {
            renderer.fillRect(x, y, 52.0f, 24.0f, SDL_Color{255, 255, 255, 255});
            renderer.fillRect(x + 16.0f, y - 12.0f, 20.0f, 16.0f, SDL_Color{255, 255, 255, 255});
        }

        if (shieldActive_)
        {
            SDL_Texture *shieldTexture = TextureManager::instance().getTexture("shield_effect");
            if (shieldTexture)
            {
                const float shieldSize = 80.0f;
                renderer.drawTexture(shieldTexture, x + (48.0f - shieldSize) / 2.0f, y + (48.0f - shieldSize) / 2.0f, shieldSize, shieldSize);
            }
        }
    }

    void Player::shoot(std::vector<Bullet> &bullets)
    {
        bullets.emplace_back(x + 22.0f, y - 14.0f, -420.0f, BulletOwner::Player);
    }

    void Player::activateFireball(std::vector<Bullet> &bullets)
    {
        if (fireballCooldown_ <= 0.0f)
        {
            Bullet &fireball = bullets.emplace_back(0.0f, 0.0f, -800.0f, BulletOwner::Player, BulletType::Fireball);
            fireball.width = 40.0f;
            fireball.height = 40.0f;
            fireball.x = x + (48.0f / 2.0f) - (fireball.width / 2.0f);
            fireball.y = y; // Start at player's y

            fireballCooldown_ = 8.0f; // 8 second cooldown
        }
    }

    void Player::activateShield()
    {
        if (shieldCooldown_ <= 0.0f)
        {
            shieldActive_ = true;
            shieldTimer_ = 5.0f;     // 5 second duration
            shieldCooldown_ = 15.0f; // 15 second cooldown
        }
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
}