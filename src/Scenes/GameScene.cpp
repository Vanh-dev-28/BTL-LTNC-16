#include "Scenes/GameScene.h"
#include "Scenes/MenuScene.h"
#include "Core/Renderer.h"
#include "Managers/TextureManager.h"
#include "Managers/FontManager.h"
#include "Managers/AudioManager.h"
#include "Managers/SceneManager.h"
#include "Utils/Constants.h"

#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace SpaceInvaders
{

    void GameScene::enter()
    {
        AudioManager::instance().playMusic("../assets/audio/music/gameplay_music.mp3");
        player_.init();
        enemyDirection_ = 1.0f;
        score_ = 0;
        gameOver_ = false;

        playerWon_ = false;
        endMenuIndex_ = 0;

        bullets_.clear();
        enemies_.clear();
        resetWave();
    }

    void GameScene::exit()
    {
        bullets_.clear();
        enemies_.clear();
        AudioManager::instance().playMusic("../assets/audio/music/background_music.mp3");
    }

    void GameScene::update(float deltaTime)
    {
        if (gameOver_)
        {
            updateEndGame();
            return;
        }

        player_.update(deltaTime, bullets_);

        updateBullets(deltaTime);
        updateEnemies(deltaTime);
        checkCollisions();

        if (allEnemiesDefeated())
        {
            gameOver_ = true;
            playerWon_ = true;
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
            renderEndGame(renderer);
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
                playerWon_ = false;
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
    void GameScene::updateEndGame()
{
    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    static bool upPressed = false;
    static bool downPressed = false;
    static bool enterPressed = false;

    // UP
    if (keyboard[SDL_SCANCODE_UP])
    {
        if (!upPressed)
        {
            endMenuIndex_--;

            if (endMenuIndex_ < 0)
            {
                endMenuIndex_ = 1;
            }
        }

        upPressed = true;
    }
    else
    {
        upPressed = false;
    }

    // DOWN
    if (keyboard[SDL_SCANCODE_DOWN])
    {
        if (!downPressed)
        {
            endMenuIndex_++;

            if (endMenuIndex_ > 1)
            {
                endMenuIndex_ = 0;
            }
        }

        downPressed = true;
    }
    else
    {
        downPressed = false;
    }

    // ENTER
    if (keyboard[SDL_SCANCODE_RETURN])
    {
        if (!enterPressed)
        {
            if (endMenuIndex_ == 0)
            {
                // REPLAY
                enter();
            }
            else
            {
                // MENU
                AudioManager::instance().playMusic(
                    "../assets/audio/music/background_music.mp3");

                if (sceneManager_ != nullptr)
                {
                    sceneManager_->changeScene(
                        std::make_unique<MenuScene>());
                }
            }
        }

        enterPressed = true;
    }
    else
    {
        enterPressed = false;
    }
}


void GameScene::renderEndGame(Renderer& renderer)
{
    SDL_Texture* popup = TextureManager::instance().getTexture("endgame_popup");
    if (popup != nullptr)
    {
        const float popupWidth = 700.0f;
        const float popupHeight = 500.0f;

        const float popupX = (Constants::SCREEN_WIDTH - popupWidth) / 2.0f;
        const float popupY = (Constants::SCREEN_HEIGHT - popupHeight) / 2.0f;

    renderer.drawTexture(
        popup,
        popupX,
        popupY,
        popupWidth,
        popupHeight);
    }

    TTF_Font* font =
        FontManager::instance().getFont("menu");

    if (font == nullptr)
    {
        return;
    }

    SDL_Color white{
        255, 255, 255, 255
    };

    SDL_Color yellow{
        255, 255, 0, 255
    };

    SDL_Color red{
        255, 80, 80, 255
    };

    renderer.drawTextCentered(
        playerWon_ ? "YOU WIN" : "GAME OVER",
        font,
        red,
        Constants::SCREEN_WIDTH / 2,
        180);


    renderer.drawTextCentered(
        "SCORE : " + std::to_string(score_),
        font,
        white,
        Constants::SCREEN_WIDTH / 2,
        280);


    SDL_Color replayColor =
        (endMenuIndex_ == 0)
            ? yellow
            : white;

    std::string replayText =
        (endMenuIndex_ == 0)
            ? "> REPLAY"
            : "REPLAY";

    renderer.drawTextCentered(
        replayText,
        font,
        replayColor,
        Constants::SCREEN_WIDTH / 2,
        390);


    SDL_Color menuColor =
        (endMenuIndex_ == 1)
            ? yellow
            : white;

    std::string menuText =
        (endMenuIndex_ == 1)
            ? "> MENU"
            : "MENU";

    renderer.drawTextCentered(
        menuText,
        font,
        menuColor,
        Constants::SCREEN_WIDTH / 2,
        460);
}
}