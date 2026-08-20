#include "Scenes/GameScene.h"

#include "Core/Input.h"
#include "Managers/AudioManager.h"
#include "Managers/SceneManager.h"
#include "Managers/TextureManager.h"
#include "Utils/Constants.h"
#include <algorithm>
#include <filesystem>

namespace SpaceInvaders
{

    GameScene::GameScene(const std::string &playerName)
        : playerName_(playerName),
          enteringPlayerName_(true)
    {
    }

    void GameScene::enter()
    {
        AudioManager::instance().playMusic(
            "../assets/audio/music/gameplay_music.mp3");

        enteringPlayerName_ = true;
        input().startTextInput();

        playerName_.clear();
        scoreSaved_ = false;

        player_.init();
        gameplayBackground_ = TextureManager::instance().getTexture("gameplay_background");
        backgroundY1_ = 0.0f;
        backgroundY2_ = -static_cast<float>(Constants::SCREEN_HEIGHT);

        enemyDirection_ = 1.0f;
        currentWave_ = 1;
        enemyFireCooldown_ = 3.0f;

        score_ = 0;

        gameOver_ = false;
        playerWon_ = false;

        inWaveTransition_ = true;
        waveTransitionTimer_ = 2.0f;

        endMenuIndex_ = 0;

        bullets_.clear();
        enemies_.clear();
        pendingEnemies_.clear();
        powerUps_.clear();
        companions_.clear();

        coneShotActive_ = false;
        coneShotTimer_ = 0.0f;

        // ===== UI =====
        const float buttonWidth = 64.0f;
        const float buttonHeight = 64.0f;

        const float topMargin = 20.0f;
        const float buttonSpacing = 40.0f;

        fireballButtonRect_ = {
            20.0f,
            topMargin,
            buttonWidth,
            buttonHeight};

        shieldButtonRect_ = {
            20.0f + buttonWidth + buttonSpacing,
            topMargin,
            buttonWidth,
            buttonHeight};

        const float pauseButtonSize = 64.0f;

        pauseButtonRect_ = {
            (Constants::SCREEN_WIDTH - pauseButtonSize) / 2.0f,
            20.0f,
            pauseButtonSize,
            pauseButtonSize};
    }

    void GameScene::exit()
    {
        input().stopTextInput();

        paused_ = false;

        bullets_.clear();
        enemies_.clear();
        pendingEnemies_.clear();
        powerUps_.clear();
        companions_.clear();

        AudioManager::instance().playMusic(
            "../assets/audio/music/background_music.mp3");
    }

    void GameScene::update(float deltaTime)
    {
        // ==========================================
        // BACKGROUND SCROLLING
        // ==========================================
        backgroundY1_ += backgroundSpeed_ * deltaTime;
        backgroundY2_ += backgroundSpeed_ * deltaTime;
        const float screenHeight = static_cast<float>(Constants::SCREEN_HEIGHT);
        if (backgroundY1_ >= screenHeight)
        {
            backgroundY1_ = backgroundY2_ - screenHeight;
        }
        if (backgroundY2_ >= screenHeight)
        {
            backgroundY2_ = backgroundY1_ - screenHeight;
        }
        // ==========================================
        // 1. PAUSE
        // ==========================================
        if (paused_)
        {
            updatePauseMenu();
            return;
        }

        // ==========================================
        // 2. PAUSE BUTTON
        // ==========================================
        const float mouseX = input().getMouseX();
        const float mouseY = input().getMouseY();

        const bool mouseOverPause =
            mouseX >= pauseButtonRect_.x &&
            mouseX <= pauseButtonRect_.x + pauseButtonRect_.w &&
            mouseY >= pauseButtonRect_.y &&
            mouseY <= pauseButtonRect_.y + pauseButtonRect_.h;

        if (mouseOverPause && input().isMousePressed(SDL_BUTTON_LEFT))
        {
            paused_ = true;
            return;
        }

        // ==========================================
        // 3. ENTER PLAYER NAME
        // ==========================================
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

        // ==========================================
        // 4. POWER-UP TIMER
        // ==========================================
        if (coneShotActive_)
        {
            coneShotTimer_ -= deltaTime;

            if (coneShotTimer_ <= 0.0f)
            {
                coneShotTimer_ = 0.0f;
                coneShotActive_ = false;
            }
        }

        // ==========================================
        // 5. GAME OVER
        // ==========================================
        if (gameOver_)
        {
            updateEndGame();
            return;
        }

        // ==========================================
        // 6. WAVE TRANSITION
        // ==========================================
        if (inWaveTransition_)
        {
            waveTransitionTimer_ -= deltaTime;

            if (waveTransitionTimer_ <= 0.0f)
            {
                inWaveTransition_ = false;
                resetWave();
            }

            player_.update(
                deltaTime,
                bullets_,
                coneShotActive_);

            updatePreviewEnemies(deltaTime);
            updateBullets(deltaTime);
            updatePowerUps(deltaTime);
            checkPowerUpCollisions();
            updateCompanion(deltaTime);
            checkCompanionCollision();

            return;
        }

        // ==========================================
        // 7. PLAYER ABILITIES
        // ==========================================
        const std::filesystem::path assetRoot = (std::filesystem::current_path() / ".." / "assets").lexically_normal();
        if (input().isKeyPressed(SDL_SCANCODE_F))
        {
            if (player_.activateFireball(bullets_))
            {
                AudioManager::instance().playSFX((assetRoot / "audio" / "powerup_sf" / "one_shot.mp3").string());
            }
        }

        if (input().isKeyPressed(SDL_SCANCODE_S))
        {
            if (player_.activateShield())
            {
                AudioManager::instance().playSFX((assetRoot / "audio" / "powerup_sf" / "shield.mp3").string());
            }
        }

        // ==========================================
        // 8. PLAYER
        // ==========================================
        player_.update(
            deltaTime,
            bullets_,
            coneShotActive_);

        // ==========================================
        // 9. GAME SYSTEMS
        // ==========================================
        updateBullets(deltaTime);
        updateEnemies(deltaTime);
        checkCollisions();
        updatePowerUps(deltaTime);
        checkPowerUpCollisions();
        updateCompanion(deltaTime);
        checkCompanionCollision();

        // ==========================================
        // 10. WAVE COMPLETE
        // ==========================================
        if (allEnemiesDefeated())
        {
            currentWave_++;

            if (currentWave_ > 5)
            {
                gameOver_ = true;
                playerWon_ = true;
                saveScore();
                return;
            }

            createFlyByPreview();
            inWaveTransition_ = true;
            waveTransitionTimer_ = 3.5f;

            enemies_.clear();
            pendingEnemies_.clear();
        }

        // ==========================================
        // 11. PLAYER DEAD
        // ==========================================
        if (!player_.isAlive())
        {
            gameOver_ = true;
            playerWon_ = false;
            saveScore();
        }
    }

    void GameScene::createFlyByPreview()
    {
        m_previewEnemies.clear();
        int numPreview = 5 + (rand() % 6);
        int heightRange = std::max(1, static_cast<int>(Constants::SCREEN_HEIGHT - 200));

        for (int i = 0; i < numPreview; ++i)
        {
            float startY = 100.0f + (rand() % heightRange);
            Vector2 startPos = {-100.0f - (i * 80.0f), startY};
            Vector2 endPos = {Constants::SCREEN_WIDTH + 100.0f, startY};

            m_previewEnemies.emplace_back(
                EnemyType::Drone,
                EnemyMovementPattern::Horizontal,
                EnemyEntryPattern::FromTop,
                startPos,
                endPos,
                800.0f);
        }
    }

    void GameScene::updatePreviewEnemies(float deltaTime)
    {
        for (auto &enemy : m_previewEnemies)
        {
            enemy.update(deltaTime, {1.0f, 0});
        }
    }

} // namespace SpaceInvaders