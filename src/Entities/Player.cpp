#include "Entities/Player.h"
#include "Entities/Bullet.h"
#include "Core/Renderer.h"
#include "Managers/TextureManager.h"
#include "Utils/Constants.h"
#include <SDL3/SDL.h>

namespace SpaceInvaders
{
    Player::Player() : x(0), y(0), speed_(0), fireCooldown_(0.18f) {}

    void Player::init()
    {
        x = Constants::SCREEN_WIDTH / 2.0f - 32.0f;
        y = Constants::SCREEN_HEIGHT - 90.0f;
        speed_ = 360.0f;
        fireCooldown_ = 0.0f;
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
    }

    void Player::shoot(std::vector<Bullet> &bullets)
    {
        bullets.emplace_back(x + 22.0f, y - 14.0f, -420.0f);
    }
}