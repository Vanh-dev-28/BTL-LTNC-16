#include "Scenes/GameScene.h"
#include "Core/Renderer.h"
#include "Managers/TextureManager.h"
#include "Managers/FontManager.h"
#include "Utils/Constants.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace SpaceInvaders
{

    void GameScene::enter()
    {
        player_.init();
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

        player_.update(deltaTime, bullets_);

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

        for (const auto &enemy : enemies_)
        {
            enemy.render(renderer);
        }

        for (const auto &bullet : bullets_)
        {
            bullet.render(renderer);
        }

        player_.render(renderer);

        if (gameOver_)
        {
            TTF_Font *font = FontManager::instance().getFont("menu");
            renderer.drawTextCentered(
                allEnemiesDefeated() ? "YOU WIN" : "GAME OVER",
                font,
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
            EnemyType type = EnemyType::Bomber; // Mặc định
            switch (row)
            {
            case 0:
                type = EnemyType::Bomber;
                break;
            case 1:
                type = EnemyType::Drone;
                break;
            case 2:
                type = EnemyType::HealthSpaceship;
                break;
            case 3:
                type = EnemyType::Drone; // Hàng cuối cùng là Drone
                break;
            }
            for (int col = 0; col < columns; ++col)
            {
                enemies_.emplace_back(
                    startX + col * spacingX,
                    startY + row * spacingY,
                    60.0f + row * 4.0f,
                    type);
            }
        }
    }

    void GameScene::updateBullets(float deltaTime)
    {
        for (auto &bullet : bullets_)
        {
            bullet.update(deltaTime);
        }
    }

    void GameScene::updateEnemies(float deltaTime)
    {
        bool hitEdge = false;
        for (auto &enemy : enemies_)
        {
            enemy.update(deltaTime, enemyDirection_);
            if (enemy.alive && (enemy.x < 20.0f || enemy.x > Constants::SCREEN_WIDTH - (enemy.width + 20.0f)))
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
            if (enemy.alive && enemy.y + enemy.height >= player_.y)
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

                const bool hit = bullet.x >= enemy.x && bullet.x <= enemy.x + enemy.width &&
                                 bullet.y >= enemy.y && bullet.y <= enemy.y + enemy.height;
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