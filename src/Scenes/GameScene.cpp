#include "Scenes/GameScene.h"

#include "Core/Input.h"
#include "Managers/AudioManager.h"
#include "Managers/SceneManager.h"
#include "Utils/Constants.h"

namespace SpaceInvaders
{

GameScene::GameScene(const std::string& playerName)
    : playerName_(playerName),
      enteringPlayerName_(true)
{
}

void GameScene::enter()
{
    AudioManager::instance().playMusic(
        "../assets/audio/music/gameplay_music.mp3"
    );

    enteringPlayerName_ = true;
    input().startTextInput();

    playerName_.clear();

    scoreSaved_ = false;

    player_.init();

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
    powerUps_.clear();
    companions_.clear();
    coneShotActive_ = false;
    coneShotTimer_ = 0.0f;

    // ===== UI =====

    const float buttonWidth = 64.0f;
    const float buttonHeight = 64.0f;

    const float topMargin = 20.0f;
    const float buttonSpacing = 20.0f;

    fireballButtonRect_ = {
        20.0f,
        topMargin,
        buttonWidth,
        buttonHeight
    };

    shieldButtonRect_ = {
        20.0f + buttonWidth + buttonSpacing,
        topMargin,
        buttonWidth,
        buttonHeight
    };

    const float pauseButtonSize = 64.0f;

    pauseButtonRect_ = {
        (Constants::SCREEN_WIDTH - pauseButtonSize) / 2.0f,
        20.0f,
        pauseButtonSize,
        pauseButtonSize
    };
}

void GameScene::exit()
{
    input().stopTextInput();

    paused_ = false;

    bullets_.clear();
    enemies_.clear();
    powerUps_.clear();
    companions_.clear();

    AudioManager::instance().playMusic(
        "../assets/audio/music/background_music.mp3"
    );
}

void GameScene::update(float deltaTime)
{
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

    if (mouseOverPause &&
        input().isMousePressed(SDL_BUTTON_LEFT))
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
            coneShotActive_
        );

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

    if (input().isKeyPressed(SDL_SCANCODE_F))
    {
        player_.activateFireball(bullets_);
    }

    if (input().isKeyPressed(SDL_SCANCODE_S))
    {
        player_.activateShield();
    }

    // ==========================================
    // 8. PLAYER
    // ==========================================

    player_.update(
        deltaTime,
        bullets_,
        coneShotActive_
    );

    // ==========================================
    // 9. GAME SYSTEMS
    // ==========================================
    updateEnemies(deltaTime);

    updatePowerUps(deltaTime);
    checkPowerUpCollisions();

    updateCompanion(deltaTime);

    updateBullets(deltaTime);

    checkCollisions();
    checkCompanionCollision();
    // ==========================================
    // 10. WAVE COMPLETE
    // ==========================================

    if (allEnemiesDefeated())
    {
        currentWave_++;

        if (currentWave_ > 3)
        {
            gameOver_ = true;
            playerWon_ = true;

            saveScore();

            return;
        }

        inWaveTransition_ = true;
        waveTransitionTimer_ = 2.0f;

        enemies_.clear();
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

} // namespace SpaceInvaders