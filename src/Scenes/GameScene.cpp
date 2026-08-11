#include "Scenes/GameScene.h"
#include "Scenes/MenuScene.h"
#include "Core/Renderer.h"
#include "Utils/Vector2.h"
#include "Managers/TextureManager.h"
#include "Managers/FontManager.h"
#include "Managers/AudioManager.h"
#include "Managers/SceneManager.h"
#include "Utils/Constants.h"

#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>

namespace SpaceInvaders
{

    void GameScene::enter()
    {
        AudioManager::instance().playMusic("../assets/audio/music/gameplay_music.mp3");
        player_.init();
        enemyDirection_ = 1.0f;
        currentWave_ = 1;
        enemyFireCooldown_ = 3.0f; // Longer initial delay for first wave
        score_ = 0;
        gameOver_ = false;
        playerWon_ = false;
        inWaveTransition_ = true;
        waveTransitionTimer_ = 2.0f; // Time for "WAVE 1" announcement

        endMenuIndex_ = 0;

        bullets_.clear();
        enemies_.clear();
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

        // Handle wave transitions
        if (inWaveTransition_)
        {
            waveTransitionTimer_ -= deltaTime;
            if (waveTransitionTimer_ <= 0.0f)
            {
                inWaveTransition_ = false;
                resetWave(); // Spawn enemies for the new wave
            }
            // We can still update the player during transition
            player_.update(deltaTime, bullets_);
            updateBullets(deltaTime);
            return; // But not enemies
        }

        player_.update(deltaTime, bullets_);

        updateBullets(deltaTime);
        updateEnemies(deltaTime);
        checkCollisions();

        if (allEnemiesDefeated())
        {
            currentWave_++;
            if (currentWave_ > 3) // Max waves reached
            {
                gameOver_ = true; // Player wins
                playerWon_ = true;
                return;
            }
            else // Prepare for next wave
            {
                inWaveTransition_ = true;
                waveTransitionTimer_ = 2.0f; // 2-second delay
                enemies_.clear();            // Clear the vector of dead enemies
            }
        }

        if (!player_.isAlive())
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

        TTF_Font *hudFont = FontManager::instance().getFont("hud_font");
        if (hudFont && player_.isAlive())
        {
            // HUD is positioned below the player
            const float playerWidth = 48.0f;
            const float playerHeight = 48.0f;
            const float hudYOffset = playerHeight + 8.0f;

            // Health Bar
            const float healthBarWidth = 80.0f;
            const float healthBarHeight = 10.0f;
            const float healthBarX = player_.x + (playerWidth - healthBarWidth) / 2.0f;
            const float healthBarY = player_.y + hudYOffset;

            // Background
            renderer.fillRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight, SDL_Color{50, 50, 50, 200});
            // Foreground
            float healthPercentage = player_.getHealth() / player_.getMaxHealth();
            if (healthPercentage > 0)
            {
                renderer.fillRect(healthBarX, healthBarY, healthBarWidth * healthPercentage, healthBarHeight, SDL_Color{40, 200, 40, 255});
            }
            // Border
            renderer.drawRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight, SDL_Color{180, 180, 180, 200});

            // Score Text
            std::string scoreText = "Score: " + std::to_string(score_);
            renderer.drawTextCentered(scoreText, hudFont, {255, 255, 255, 255}, healthBarX + healthBarWidth / 2, healthBarY + healthBarHeight + 2.0f);
        }

        // Render current wave in top-right corner
        if (hudFont)
        {
            std::string waveText = "WAVE " + std::to_string(currentWave_);
            int textWidth, textHeight;
            renderer.measureText(waveText, hudFont, textWidth, textHeight);
            renderer.drawText(waveText, hudFont, {255, 255, 255, 200}, Constants::SCREEN_WIDTH - textWidth - 20, 20);
        }

        // Wave Announcement
        if (inWaveTransition_ && currentWave_ <= 3)
        {
            TTF_Font *titleFont = FontManager::instance().getFont("menu_title");
            if (titleFont)
            {
                std::string announcementText = "WAVE " + std::to_string(currentWave_);
                renderer.drawTextCentered(announcementText, titleFont, {255, 255, 0, 255}, Constants::SCREEN_WIDTH / 2, Constants::SCREEN_HEIGHT / 2 - 100);
            }
        }

        if (gameOver_)
        {
            TTF_Font *font = FontManager::instance().getFont("menu");
            renderer.drawTextCentered(
                playerWon_ ? "YOU WIN" : "GAME OVER",
                font,
                SDL_Color{255, 80, 80, 255},
                Constants::SCREEN_WIDTH / 2,
                Constants::SCREEN_HEIGHT / 2);
            renderEndGame(renderer);
        }
    }

    void GameScene::resetWave()
    {
        enemies_.clear();
        bullets_.clear();
        enemyDirection_ = 1.0f;

        switch (currentWave_)
        {
        case 1:
        {
            // Wave 1: From Top
            const int columns = 8;
            const int rows = 3;
            const float speed = 60.0f;
            for (int row = 0; row < rows; ++row)
            {
                for (int col = 0; col < columns; ++col)
                {
                    Vector2 targetPos = {150.0f + col * 80.0f, 100.0f + row * 60.0f};
                    Vector2 startPos = {targetPos.x, -50.0f - row * 60.0f}; // Start from above the screen
                    enemies_.emplace_back(
                        EnemyType::Drone,
                        EnemyMovementPattern::Horizontal,
                        EnemyEntryPattern::FromTop,
                        startPos,
                        targetPos,
                        speed);
                }
            }
            break;
        }
        case 2:
        {
            // Wave 2: Arc from sides
            const int perSide = 8;
            const float speed = 75.0f;
            for (int i = 0; i < perSide; ++i)
            {
                // Left side
                Vector2 targetPosL = {200.0f + (i % 4) * 100.0f, 120.0f + (i / 4) * 70.0f};
                Vector2 startPosL = {-100.0f, 100.0f + i * 20.0f};
                enemies_.emplace_back(
                    EnemyType::Bomber,
                    EnemyMovementPattern::SineWave,
                    EnemyEntryPattern::ArcFromLeft,
                    startPosL,
                    targetPosL,
                    speed);

                // Right side
                Vector2 targetPosR = {Constants::SCREEN_WIDTH - 200.0f - (i % 4) * 100.0f, 120.0f + (i / 4) * 70.0f};
                Vector2 startPosR = {Constants::SCREEN_WIDTH + 100.0f, 100.0f + i * 20.0f};
                enemies_.emplace_back(
                    EnemyType::Bomber,
                    EnemyMovementPattern::SineWave,
                    EnemyEntryPattern::ArcFromRight,
                    startPosR,
                    targetPosR,
                    speed);
            }
            break;
        }
        case 3:
        {
            // Wave 3: Mixed
            const float speed = 90.0f;
            // From Top group
            for (int i = 0; i < 5; ++i)
            {
                Vector2 targetPos = {340.0f + i * 120.0f, 250.0f};
                Vector2 startPos = {targetPos.x, -50.0f};
                enemies_.emplace_back(
                    EnemyType::HealthSpaceship,
                    EnemyMovementPattern::Horizontal,
                    EnemyEntryPattern::FromTop,
                    startPos,
                    targetPos,
                    speed);
            }
            // Arc from sides group
            for (int i = 0; i < 6; ++i)
            {
                // Left
                Vector2 targetPosL = {150.0f + (i % 3) * 100.0f, 100.0f + (i / 3) * 60.0f};
                Vector2 startPosL = {-100.0f, 150.0f + i * 15.0f};
                enemies_.emplace_back(
                    EnemyType::Drone,
                    EnemyMovementPattern::SineWave,
                    EnemyEntryPattern::ArcFromLeft,
                    startPosL,
                    targetPosL,
                    speed);

                // Right
                Vector2 targetPosR = {Constants::SCREEN_WIDTH - 150.0f - (i % 3) * 100.0f, 100.0f + (i / 3) * 60.0f};
                Vector2 startPosR = {Constants::SCREEN_WIDTH + 100.0f, 150.0f + i * 15.0f};
                enemies_.emplace_back(
                    EnemyType::Drone,
                    EnemyMovementPattern::SineWave,
                    EnemyEntryPattern::ArcFromRight,
                    startPosR,
                    targetPosR,
                    speed);
            }
            break;
        }
        }
    }

    void GameScene::updateBullets(float deltaTime)
    {
        for (auto &bullet : bullets_)
        {
            bullet.update(deltaTime);
        }

        // Remove inactive bullets
        bullets_.erase(
            std::remove_if(bullets_.begin(), bullets_.end(), [](const Bullet &b)
                           { return !b.active; }),
            bullets_.end());
    }

    void GameScene::updateEnemies(float deltaTime)
    {
        // --- Enemy Shooting Logic ---
        enemyFireCooldown_ -= deltaTime;
        if (enemyFireCooldown_ <= 0.0f && !gameOver_)
        {
            std::vector<int> livingEnemyIndices;
            for (int i = 0; i < enemies_.size(); ++i)
            {
                if (enemies_[i].alive)
                {
                    livingEnemyIndices.push_back(i);
                }
            }

            if (!livingEnemyIndices.empty())
            {
                int shooterIndex = livingEnemyIndices[rand() % livingEnemyIndices.size()];
                const auto &shooter = enemies_[shooterIndex];
                // Spawn bullet from the center of the enemy
                bullets_.emplace_back(shooter.x + shooter.width / 2.0f - 2.0f, shooter.y + shooter.height, 250.0f, BulletOwner::Enemy);
            }

            // Fire rate increases with waves
            float baseCooldown = 1.5f - (currentWave_ * 0.25f);                                 // Wave 1: 1.25, Wave 2: 1.0, Wave 3: 0.75
            enemyFireCooldown_ = baseCooldown + (static_cast<float>(rand()) / RAND_MAX) * 0.5f; // Add some randomness
        }

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

            if (bullet.owner == BulletOwner::Player)
            {
                // Check collision with enemies
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
                        break; // Một viên đạn chỉ trúng 1 kẻ địch
                    }
                }
            }
            else // bullet.owner == BulletOwner::Enemy
            {
                // Check collision with player
                if (player_.isAlive())
                {
                    const float playerWidth = 48.0f;
                    const float playerHeight = 48.0f;
                    const bool hit = bullet.x >= player_.x && bullet.x <= player_.x + playerWidth &&
                                     bullet.y >= player_.y && bullet.y <= player_.y + playerHeight;
                    if (hit)
                    {
                        player_.takeDamage(Constants::ENEMY_LASER_DAMAGE);
                        bullet.active = false;
                    }
                }
            }
        }
    }

    bool GameScene::allEnemiesDefeated() const
    {
        // If we are in transition, or the wave hasn't spawned enemies yet,
        // then they are not "all defeated" in a way that should trigger the next wave.
        if (inWaveTransition_ || enemies_.empty())
        {
            return false;
        }

        // Check if any enemy is still alive.
        for (const auto &enemy : enemies_)
        {
            if (enemy.alive)
            {
                return false; // Found a live one.
            }
        }

        // If we got here, no enemies were alive.
        return true;
    }
    void GameScene::updateEndGame()
    {
        const bool *keyboard = SDL_GetKeyboardState(nullptr);

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

    void GameScene::renderEndGame(Renderer &renderer)
    {
        SDL_Texture *popup = TextureManager::instance().getTexture("endgame_popup");
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

        TTF_Font *font =
            FontManager::instance().getFont("menu");

        if (font == nullptr)
        {
            return;
        }

        SDL_Color white{
            255, 255, 255, 255};

        SDL_Color yellow{
            255, 255, 0, 255};

        SDL_Color red{
            255, 80, 80, 255};

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