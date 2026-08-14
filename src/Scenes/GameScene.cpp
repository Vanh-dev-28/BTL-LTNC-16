#include "Scenes/GameScene.h"
#include "Scenes/MenuScene.h"
#include "Core/Renderer.h"
#include "Utils/Vector2.h"
#include "Managers/TextureManager.h"
#include "Core/Input.h"
#include "Managers/FontManager.h"
#include "Managers/AudioManager.h"
#include "Managers/SceneManager.h"
#include "Managers/RankingManager.h"
#include "Utils/Constants.h"

#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>

namespace SpaceInvaders
{
    GameScene::GameScene(const std::string& playerName)
    : playerName_(playerName),
      enteringPlayerName_(true)
    {
    }

    void GameScene::enter()
    {
        AudioManager::instance().playMusic("../assets/audio/music/gameplay_music.mp3");
        enteringPlayerName_ = true;
        input().startTextInput();

        playerName_.clear();
        scoreSaved_ = false;
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

        // --- UI Setup ---
        const float buttonWidth = 64.0f;  // Adjusted for smaller top-left icons
        const float buttonHeight = 64.0f; // Adjusted for smaller top-left icons
        const float topMargin = 20.0f;
        const float buttonSpacing = 20.0f;
        fireballButtonRect_ = {20.0f, topMargin, buttonWidth, buttonHeight};                             // Top-left position
        shieldButtonRect_ = {20.0f + buttonWidth + buttonSpacing, topMargin, buttonWidth, buttonHeight}; // Next to Fireball
    }

    void GameScene::exit()
    {
        input().stopTextInput();
        bullets_.clear();
        enemies_.clear();
        AudioManager::instance().playMusic("../assets/audio/music/background_music.mp3");
    }

    void GameScene::update(float deltaTime)
    {
        if (enteringPlayerName_)
        {   
            playerName_ = input().getTextInput();

            if (input().isKeyPressed(SDL_SCANCODE_RETURN))
            {
                if (!playerName_.empty())
                {
                    enteringPlayerName_ = false;
                    input().clearTextInput();
                    input().stopTextInput();
                }
            }

            return;
        }
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

        // --- Handle Ability Input ---
        if (input().isKeyPressed(SDL_SCANCODE_F))
        {
            player_.activateFireball(bullets_);
        }
        if (input().isKeyPressed(SDL_SCANCODE_S))
        {
            player_.activateShield();
        }
        // --- End Handle Ability Input ---

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

                saveScore();
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
            playerWon_ = false;
            saveScore();
        }
    }

    void GameScene::render(Renderer &renderer)
    {
        TTF_Font *font =
        FontManager::instance().getFont("menu");

    if (enteringPlayerName_)
    {
        SDL_Texture* background = TextureManager::instance().getTexture("entername_background");
        if (background != nullptr)
        {
            renderer.drawTexture(
                background,
                0.0f,
                0.0f,
                static_cast<float>(Constants::SCREEN_WIDTH),
                static_cast<float>(Constants::SCREEN_HEIGHT)
            );
        }
        else
        {
            SDL_SetRenderDrawColor(
                renderer.getSDLRenderer(),
                10, 20, 40, 255
            );

            SDL_RenderClear(renderer.getSDLRenderer());
        }

        if (font != nullptr)
        {
            renderer.drawTextCentered(
                "ENTER YOUR NAME",
                font,
                {255, 230, 50, 255},
                Constants::SCREEN_WIDTH / 2,
                Constants::SCREEN_HEIGHT / 2 - 80
            );

            renderer.drawTextCentered(
                playerName_.empty()
                    ? "_"
                    : playerName_,
                font,
                {255, 255, 255, 255},
                Constants::SCREEN_WIDTH / 2,
                Constants::SCREEN_HEIGHT / 2
            );

            renderer.drawTextCentered(
                "PRESS ENTER TO START",
                font,
                {50, 230, 255, 255},
                Constants::SCREEN_WIDTH / 2,
                Constants::SCREEN_HEIGHT / 2 + 80
            );
        }

        return;
    }

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
            renderEndGame(renderer);
        }

        // --- Render Ability Buttons ---
        if (!gameOver_)
        {
            SDL_Color white{255, 255, 255, 200};
            SDL_Texture *fireballIcon = TextureManager::instance().getTexture("fireball_icon");
            SDL_Texture *shieldIcon = TextureManager::instance().getTexture("shield_icon");

            // Fireball Button
            if (fireballIcon)
            {
                renderer.drawTexture(fireballIcon, fireballButtonRect_.x, fireballButtonRect_.y, fireballButtonRect_.w, fireballButtonRect_.h);
            }
            else // Fallback
            {
                renderer.fillRect(fireballButtonRect_.x, fireballButtonRect_.y, fireballButtonRect_.w, fireballButtonRect_.h, {100, 50, 0, 255});
                renderer.drawTextCentered("F", hudFont, {255, 255, 255, 255}, fireballButtonRect_.x + fireballButtonRect_.w / 2, fireballButtonRect_.y + fireballButtonRect_.h / 2 - 10);
            }
            // Cooldown overlay
            float fireballCD = player_.getFireballCooldownRatio();
            if (fireballCD > 0.0f)
            {
                renderer.fillRect(fireballButtonRect_.x, fireballButtonRect_.y, fireballButtonRect_.w, fireballButtonRect_.h * fireballCD, {0, 0, 0, 180});
            }
            renderer.drawTextCentered(
                "Press F",
                hudFont,
                white,
                fireballButtonRect_.x + fireballButtonRect_.w / 2, fireballButtonRect_.y + fireballButtonRect_.h + 10); // Adjusted Y for text

            // Shield Button
            if (shieldIcon)
            {
                renderer.drawTexture(shieldIcon, shieldButtonRect_.x, shieldButtonRect_.y, shieldButtonRect_.w, shieldButtonRect_.h);
            }
            else // Fallback
            {
                renderer.fillRect(shieldButtonRect_.x, shieldButtonRect_.y, shieldButtonRect_.w, shieldButtonRect_.h, {100, 100, 0, 255});
                renderer.drawTextCentered("S", hudFont, {255, 255, 255, 255}, shieldButtonRect_.x + shieldButtonRect_.w / 2, shieldButtonRect_.y + shieldButtonRect_.h / 2 - 10);
            }
            // Cooldown/Active overlay
            if (player_.isShieldActive())
            {
                renderer.drawRect(shieldButtonRect_.x, shieldButtonRect_.y, shieldButtonRect_.w, shieldButtonRect_.h, {0, 255, 0, 255});
                renderer.fillRect(shieldButtonRect_.x, shieldButtonRect_.y + shieldButtonRect_.h * (1.0f - player_.getShieldTimeRatio()), shieldButtonRect_.w, shieldButtonRect_.h * player_.getShieldTimeRatio(), {0, 255, 0, 100});
            }
            else if (player_.getShieldCooldownRatio() > 0.0f)
            {
                renderer.fillRect(shieldButtonRect_.x, shieldButtonRect_.y, shieldButtonRect_.w, shieldButtonRect_.h * player_.getShieldCooldownRatio(), {0, 0, 0, 180});
            }
            renderer.drawTextCentered(
                "Press S",
                hudFont,
                white,
                shieldButtonRect_.x + shieldButtonRect_.w / 2, shieldButtonRect_.y + shieldButtonRect_.h + 10); // Adjusted Y for text
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
                bullets_.emplace_back(shooter.x, shooter.y + shooter.height, 250.0f, BulletOwner::Enemy);

                // Lấy tham chiếu đến viên đạn vừa tạo và tùy chỉnh kích thước
                Bullet &newBullet = bullets_.back();
                newBullet.width = 24.0f;  // Kích thước chiều rộng mới
                newBullet.height = 48.0f; // Kích thước chiều dài mới

                // Căn chỉnh lại vị trí để đạn bắn ra từ giữa tàu địch
                newBullet.x = shooter.x + (shooter.width / 2.0f) - (newBullet.width / 2.0f);
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

                    // AABB collision check (rectangle-rectangle)
                    const bool hit = bullet.x < enemy.x + enemy.width &&
                                     bullet.x + bullet.width > enemy.x &&
                                     bullet.y < enemy.y + enemy.height &&
                                     bullet.y + bullet.height > enemy.y;
                    if (hit)
                    {
                        enemy.alive = false;
                        score_ += 10;

                        // Normal bullets are destroyed on impact, Fireball is not.
                        if (bullet.type != BulletType::Fireball)
                        {
                            bullet.active = false;
                            break; // A normal bullet only hits one enemy
                        }
                    }
                }
            }
            else // bullet.owner == BulletOwner::Enemy
            {
                // Check collision with player
                if (player_.isAlive())
                {
                    // Check collision with shield first
                    if (player_.isShieldActive())
                    {
                        const float shieldSize = 80.0f;
                        const float shieldX = player_.x + (48.0f - shieldSize) / 2.0f;
                        const float shieldY = player_.y + (48.0f - shieldSize) / 2.0f;
                        const bool shieldHit = bullet.x < shieldX + shieldSize &&
                                               bullet.x + bullet.width > shieldX &&
                                               bullet.y < shieldY + shieldSize &&
                                               bullet.y + bullet.height > shieldY;
                        if (shieldHit)
                        {
                            bullet.active = false;
                            continue; // Bullet destroyed, go to next bullet
                        }
                    }
                    const float playerWidth = 48.0f;
                    const float playerHeight = 48.0f;
                    // AABB collision check (rectangle-rectangle)
                    const bool hit = bullet.x < player_.x + playerWidth &&
                                     bullet.x + bullet.width > player_.x &&
                                     bullet.y < player_.y + playerHeight &&
                                     bullet.y + bullet.height > player_.y;
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
        float mouseX;
        float mouseY;

        SDL_MouseButtonFlags mouseState = SDL_GetMouseState(&mouseX, &mouseY);

        if (Renderer::s_scale > 0.0f)
        {
            mouseX = (mouseX - Renderer::s_offsetX) / Renderer::s_scale;

            mouseY = (mouseY - Renderer::s_offsetY) / Renderer::s_scale;
        }
        static bool mousePressed = false;

        bool leftClick = (mouseState & SDL_BUTTON_LMASK) != 0;

        if (mouseX >= replayButtonRect_.x &&
        mouseX <= replayButtonRect_.x + replayButtonRect_.w &&
        mouseY >= replayButtonRect_.y &&
        mouseY <= replayButtonRect_.y + replayButtonRect_.h)
        {
            endMenuIndex_ = 0;
        }
        else if (mouseX >= menuButtonRect_.x &&
            mouseX <= menuButtonRect_.x + menuButtonRect_.w &&
            mouseY >= menuButtonRect_.y &&
            mouseY <= menuButtonRect_.y + menuButtonRect_.h)
        {
            endMenuIndex_ = 1;
        }

        if (leftClick && !mousePressed)
        {
        
            if (mouseX >= replayButtonRect_.x &&
                mouseX <= replayButtonRect_.x + replayButtonRect_.w &&
                mouseY >= replayButtonRect_.y &&
                mouseY <= replayButtonRect_.y + replayButtonRect_.h)
            {
                endMenuIndex_ = 0;
                enter();
            }
        

            else if (mouseX >= menuButtonRect_.x &&
                mouseX <= menuButtonRect_.x + menuButtonRect_.w &&
                mouseY >= menuButtonRect_.y &&
                mouseY <= menuButtonRect_.y + menuButtonRect_.h)
            {
                endMenuIndex_ = 1;

                AudioManager::instance().playMusic("../assets/audio/music/background_music.mp3");

                if (sceneManager_ != nullptr)
                {
                    sceneManager_->changeScene(std::make_unique<MenuScene>());
                }
            }
        }

        mousePressed = leftClick;
    }

    void GameScene::renderEndGame(Renderer &renderer)
{
    SDL_Texture *popup = TextureManager::instance().getTexture("endgame_popup");
    if (popup == nullptr)
    {
        return;
    }

    const float popupWidth = 700.0f;
    const float popupHeight = popupWidth * 320.0f / 700.0f; 

    const float popupX =
        (Constants::SCREEN_WIDTH - popupWidth) / 2.0f;

    const float popupY =
        (Constants::SCREEN_HEIGHT - popupHeight) / 2.0f;

    if (popup != nullptr)
    {
        renderer.drawTexture(
            popup,
            popupX,
            popupY,
            popupWidth,
            popupHeight);
    }
    const float sx = popupWidth / 700.0f;
    const float sy = popupHeight / 320.0f;

    constexpr float buttonWidth = 220.0f;
    constexpr float buttonHeight = 50.0f;
    constexpr float buttonY = 195.0f;

    replayButtonRect_ = {
        popupX + 100.0f * sx,
        popupY + buttonY * sy,
        buttonWidth * sx,
        buttonHeight * sy
    };

    menuButtonRect_ = {
        popupX + 380.0f * sx,
        popupY + buttonY * sy,
        buttonWidth * sx,
        buttonHeight * sy
    };

    TTF_Font *font =
        FontManager::instance().getFont("menu");

    if (font == nullptr)
    {
        return;
    }

    SDL_Color white{255, 255, 255, 255};
    SDL_Color yellow{255, 255, 0, 255};
    SDL_Color red{255, 80, 80, 255};


    renderer.drawTextCentered(
        playerWon_ ? "YOU WIN" : "GAME OVER",
        font,
        red,
        static_cast<int>(popupX + 350.0f * sx),
        popupY + 55.0f * sy
    );


    renderer.drawTextCentered(
    "PLAYER : " + playerName_,
    font,
    white,
    Constants::SCREEN_WIDTH / 2,
    popupY + 100.0f * sy);

    renderer.drawTextCentered(
    "SCORE : " + std::to_string(score_),
    font,
    white,
    Constants::SCREEN_WIDTH / 2,
    popupY + 150.0f * sy);


    SDL_Color replayColor =
        (endMenuIndex_ == 0) ? yellow : white;

    std::string replayText =
        (endMenuIndex_ == 0) ? "> REPLAY" : "REPLAY";

    renderer.drawTextCentered(
        replayText,
        font,
        replayColor,
        popupX + 210.0f * sx,
        popupY + 215.0f * sy
    );

    SDL_Color menuColor =
        (endMenuIndex_ == 1) ? yellow : white;

    std::string menuText =
        (endMenuIndex_ == 1) ? "> MENU" : "MENU";

    renderer.drawTextCentered(
        menuText,
        font,
        menuColor,
        popupX + 490.0f * sx,
        popupY + 215.0f * sy
    );
    }

    void GameScene::saveScore()
{
    if (scoreSaved_)
        return;

    RankingManager::instance().addScore(playerName_, score_);

    if (!RankingManager::instance().save("../assets/data/ranking.txt"))
    {
        SDL_Log("Failed to save ranking!");
    }

    scoreSaved_ = true;
}
    
};