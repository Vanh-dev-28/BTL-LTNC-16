#include "Scenes/GameScene.h"
#include "Core/Renderer.h"
#include "Managers/TextureManager.h"
#include "Utils/Constants.h"

#include <SDL3/SDL.h>

namespace SpaceInvaders
{

    void GameScene::enter()
    {
        playerX_ = Constants::SCREEN_WIDTH / 2.0f - 32.0f;
        playerY_ = Constants::SCREEN_HEIGHT - 90.0f;
        playerSpeed_ = 360.0f;
        fireCooldown_ = 0.0f;
        enemyDirection_ = 1.0f;
        score_ = 0;
        gameOver_ = false;
        bullets_.clear();
        enemies_.clear();
        resetWave();
    }

    void GameScene::exit()
    {
        bullets_.clear();
        enemies_.clear();
    }

    void GameScene::update(float deltaTime)
    {
        if (gameOver_)
        {
            return;
        }

        const bool *keyboardState = SDL_GetKeyboardState(nullptr);

        if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT])
        {
            playerX_ -= playerSpeed_ * deltaTime;
        }
        if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT])
        {
            playerX_ += playerSpeed_ * deltaTime;
        }

        if (playerX_ < 20.0f)
        {
            playerX_ = 20.0f;
        }
        if (playerX_ > Constants::SCREEN_WIDTH - 64.0f)
        {
            playerX_ = Constants::SCREEN_WIDTH - 64.0f;
        }

        fireCooldown_ -= deltaTime;
        if (fireCooldown_ <= 0.0f && (keyboardState[SDL_SCANCODE_SPACE] || keyboardState[SDL_SCANCODE_J]))
        {
            spawnBullet();
            fireCooldown_ = 0.18f;
        }

        updateBullets(deltaTime);
        updateEnemies(deltaTime);
        checkCollisions();

        if (allEnemiesDefeated())
        {
            gameOver_ = true;
        }
    }

    void GameScene::render(Renderer &renderer)
    {
        SDL_Texture *background = TextureManager::instance().getTexture("gameplay_background");
        if (background != nullptr)
        {
            renderer.drawTexture(
                background,
                0.0f,
                0.0f,
                static_cast<float>(Constants::SCREEN_WIDTH),
                static_cast<float>(Constants::SCREEN_HEIGHT));
        }
        else
        {
            SDL_SetRenderDrawColor(renderer.getSDLRenderer(), 10, 20, 40, 255);
            SDL_RenderClear(renderer.getSDLRenderer());
        }

        SDL_Texture *enemyTexture = TextureManager::instance().getTexture("bugs_invaders");
        for (const auto &enemy : enemies_)
        {
            if (!enemy.alive)
            {
                continue;
            }

            if (enemyTexture != nullptr)
            {
                renderer.drawTexture(enemyTexture, enemy.x, enemy.y, 42.0f, 42.0f);
            }
            else
            {
                renderer.fillRect(enemy.x, enemy.y, 42.0f, 42.0f, SDL_Color{255, 80, 80, 255});
            }
        }

        for (const auto &bullet : bullets_)
        {
            if (bullet.active)
            {
                renderer.fillRect(bullet.x, bullet.y, 4.0f, 14.0f, SDL_Color{255, 255, 120, 255});
            }
        }

        SDL_Texture *playerTexture = TextureManager::instance().getTexture("ship");
        if (playerTexture != nullptr)
        {
            renderer.drawTexture(playerTexture, playerX_, playerY_, 48.0f, 48.0f);
        }
        else
        {
            // Fallback to drawing rectangles if the texture isn't found
            renderer.fillRect(playerX_, playerY_, 52.0f, 24.0f, SDL_Color{255, 255, 255, 255});
            renderer.fillRect(playerX_ + 16.0f, playerY_ - 12.0f, 20.0f, 16.0f, SDL_Color{255, 255, 255, 255});
        }

        if (gameOver_)
        {
            renderer.drawTextCentered(
                allEnemiesDefeated() ? "YOU WIN" : "GAME OVER",
                nullptr,
                SDL_Color{255, 80, 80, 255},
                Constants::SCREEN_WIDTH / 2,
                Constants::SCREEN_HEIGHT / 2);
        }
    }

    void GameScene::resetWave()
    {
        const int columns = 8;
        const int rows = 4;
        const float spacingX = 72.0f;
        const float spacingY = 58.0f;
        const float startX = 70.0f;
        const float startY = 50.0f;

        enemies_.clear();
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < columns; ++col)
            {
                Enemy enemy{};
                enemy.x = startX + col * spacingX;
                enemy.y = startY + row * spacingY;
                enemy.speed = 60.0f + row * 4.0f;
                enemy.alive = true;
                enemies_.push_back(enemy);
            }
        }
    }

    void GameScene::spawnBullet()
    {
        bullets_.push_back(Bullet{playerX_ + 22.0f, playerY_ - 14.0f, -420.0f, true});
    }

    void GameScene::updateBullets(float deltaTime)
    {
        for (auto &bullet : bullets_)
        {
            if (!bullet.active)
            {
                continue;
            }

            bullet.y += bullet.speed * deltaTime;
            if (bullet.y < -20.0f || bullet.y > Constants::SCREEN_HEIGHT + 20.0f)
            {
                bullet.active = false;
            }
        }
    }

    void GameScene::updateEnemies(float deltaTime)
    {
        bool hitEdge = false;
        for (auto &enemy : enemies_)
        {
            if (!enemy.alive)
            {
                continue;
            }

            enemy.x += enemyDirection_ * enemy.speed * deltaTime;
            if (enemy.x < 20.0f || enemy.x > Constants::SCREEN_WIDTH - 58.0f)
            {
                hitEdge = true;
            }
        }

        if (hitEdge)
        {
            enemyDirection_ *= -1.0f;
            for (auto &enemy : enemies_)
            {
                if (enemy.alive)
                {
                    enemy.y += 18.0f;
                }
            }
        }

        for (const auto &enemy : enemies_)
        {
            if (enemy.alive && enemy.y + 42.0f >= playerY_)
            {
                gameOver_ = true;
                break;
            }
        }
    }

    void GameScene::checkCollisions()
    {
        for (auto &bullet : bullets_)
        {
            if (!bullet.active)
            {
                continue;
            }

            for (auto &enemy : enemies_)
            {
                if (!enemy.alive)
                {
                    continue;
                }

                const bool hit = bullet.x >= enemy.x && bullet.x <= enemy.x + 42.0f &&
                                 bullet.y >= enemy.y && bullet.y <= enemy.y + 42.0f;
                if (hit)
                {
                    enemy.alive = false;
                    bullet.active = false;
                    score_ += 10;
                    break;
                }
            }
        }
    }

    bool GameScene::allEnemiesDefeated() const
    {
        for (const auto &enemy : enemies_)
        {
            if (enemy.alive)
            {
                return false;
            }
        }
        return true;
    }

}