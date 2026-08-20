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
#include "Entities/PowerUp.h"
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>

namespace SpaceInvaders
{
    void GameScene::render(Renderer &renderer)
    {
        TTF_Font *font = FontManager::instance().getFont("menu");
        if (enteringPlayerName_)
        {
            SDL_Texture *background = TextureManager::instance().getTexture("entername_background");
            if (background != nullptr)
            {
                renderer.drawTexture(background, 0.0f, 0.0f, static_cast<float>(Constants::SCREEN_WIDTH), static_cast<float>(Constants::SCREEN_HEIGHT));
            }
            else
            {
                SDL_SetRenderDrawColor(renderer.getSDLRenderer(), 10, 20, 40, 255);
                SDL_RenderClear(renderer.getSDLRenderer());
            }

            if (font != nullptr)
            {
                renderer.drawTextCentered("ENTER YOUR NAME", font, {255, 230, 50, 255}, Constants::SCREEN_WIDTH / 2, Constants::SCREEN_HEIGHT / 2 - 80);
                renderer.drawTextCentered(playerName_.empty() ? "_" : playerName_, font, {255, 255, 255, 255}, Constants::SCREEN_WIDTH / 2, Constants::SCREEN_HEIGHT / 2);
                renderer.drawTextCentered("PRESS ENTER TO START", font, {50, 230, 255, 255}, Constants::SCREEN_WIDTH / 2, Constants::SCREEN_HEIGHT / 2 + 80);
            }
            return;
        }

        SDL_Texture *background = TextureManager::instance().getTexture("gameplay_background");
        if (background != nullptr)
        {
            const float screenWidth = static_cast<float>(Constants::SCREEN_WIDTH);
            const float screenHeight = static_cast<float>(Constants::SCREEN_HEIGHT);
            renderer.drawTexture(background, 0.0f, backgroundY1_, screenWidth, screenHeight);
            renderer.drawTexture(background, 0.0f, backgroundY2_, screenWidth, screenHeight);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer.getSDLRenderer(), 10, 20, 40, 255);
            SDL_RenderClear(renderer.getSDLRenderer());
        }

        for (const auto &enemy : m_previewEnemies)
        {
            enemy.render(renderer);
        }
        for (const auto &enemy : enemies_)
        {
            enemy.render(renderer);
        }
        for (const auto &powerUp : powerUps_)
        {
            powerUp.render(renderer);
        }
        renderCompanion(renderer);

        for (const auto &bullet : bullets_)
        {
            bullet.render(renderer);
        }
        player_.render(renderer);

        TTF_Font *hudFont = FontManager::instance().getFont("hud_font");
        if (hudFont && player_.isAlive())
        {
            const float playerWidth = 48.0f;
            const float playerHeight = 48.0f;
            const float hudYOffset = playerHeight + 8.0f;

            const float healthBarWidth = 80.0f;
            const float healthBarHeight = 10.0f;
            const float healthBarX = player_.x + (playerWidth - healthBarWidth) / 2.0f;
            const float healthBarY = player_.y + hudYOffset;

            renderer.fillRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight, SDL_Color{50, 50, 50, 200});
            float healthPercentage = player_.getHealth() / player_.getMaxHealth();
            if (healthPercentage > 0)
            {
                renderer.fillRect(healthBarX, healthBarY, healthBarWidth * healthPercentage, healthBarHeight, SDL_Color{40, 200, 40, 255});
            }
            renderer.drawRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight, SDL_Color{180, 180, 180, 200});

            std::string scoreText = "Score: " + std::to_string(score_);
            renderer.drawTextCentered(scoreText, hudFont, {255, 255, 255, 255}, healthBarX + healthBarWidth / 2, healthBarY + healthBarHeight + 2.0f);
        }

        if (hudFont)
        {
            std::string waveText = "WAVE " + std::to_string(currentWave_);
            int textWidth, textHeight;
            renderer.measureText(waveText, hudFont, textWidth, textHeight);
            renderer.drawText(waveText, hudFont, {255, 255, 255, 200}, Constants::SCREEN_WIDTH - textWidth - 20, 20);
        }

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

        if (!gameOver_)
        {
            SDL_Color white{255, 255, 255, 200};
            SDL_Texture *fireballIcon = TextureManager::instance().getTexture("fireball_icon");
            SDL_Texture *shieldIcon = TextureManager::instance().getTexture("shield_icon");
            SDL_Texture *pauseIcon = TextureManager::instance().getTexture("pause_icon");

            if (pauseIcon != nullptr)
            {
                renderer.drawTexture(pauseIcon, pauseButtonRect_.x, pauseButtonRect_.y, pauseButtonRect_.w, pauseButtonRect_.h);
            }

            if (fireballIcon)
            {
                renderer.drawTexture(fireballIcon, fireballButtonRect_.x, fireballButtonRect_.y, fireballButtonRect_.w, fireballButtonRect_.h);
            }
            else
            {
                renderer.fillRect(fireballButtonRect_.x, fireballButtonRect_.y, fireballButtonRect_.w, fireballButtonRect_.h, {100, 50, 0, 255});
                renderer.drawTextCentered("F", hudFont, {255, 255, 255, 255}, fireballButtonRect_.x + fireballButtonRect_.w / 2, fireballButtonRect_.y + fireballButtonRect_.h / 2 - 10);
            }
            float fireballCD = player_.getFireballCooldownRatio();
            if (fireballCD > 0.0f)
            {
                renderer.fillRect(fireballButtonRect_.x, fireballButtonRect_.y, fireballButtonRect_.w, fireballButtonRect_.h * fireballCD, {0, 0, 0, 180});
            }
            renderer.drawTextCentered("Press F", hudFont, white, fireballButtonRect_.x + fireballButtonRect_.w / 2, fireballButtonRect_.y + fireballButtonRect_.h + 10);

            if (shieldIcon)
            {
                renderer.drawTexture(shieldIcon, shieldButtonRect_.x, shieldButtonRect_.y, shieldButtonRect_.w, shieldButtonRect_.h);
            }
            else
            {
                renderer.fillRect(shieldButtonRect_.x, shieldButtonRect_.y, shieldButtonRect_.w, shieldButtonRect_.h, {100, 100, 0, 255});
                renderer.drawTextCentered("S", hudFont, {255, 255, 255, 255}, shieldButtonRect_.x + shieldButtonRect_.w / 2, shieldButtonRect_.y + shieldButtonRect_.h / 2 - 10);
            }

            if (player_.isShieldActive())
            {
                renderer.drawRect(shieldButtonRect_.x, shieldButtonRect_.y, shieldButtonRect_.w, shieldButtonRect_.h, {0, 255, 0, 255});
                renderer.fillRect(shieldButtonRect_.x, shieldButtonRect_.y + shieldButtonRect_.h * (1.0f - player_.getShieldTimeRatio()), shieldButtonRect_.w, shieldButtonRect_.h * player_.getShieldTimeRatio(), {0, 255, 0, 100});
            }
            else if (player_.getShieldCooldownRatio() > 0.0f)
            {
                renderer.fillRect(shieldButtonRect_.x, shieldButtonRect_.y, shieldButtonRect_.w, shieldButtonRect_.h * player_.getShieldCooldownRatio(), {0, 0, 0, 180});
            }
            renderer.drawTextCentered("Press S", hudFont, white, shieldButtonRect_.x + shieldButtonRect_.w / 2, shieldButtonRect_.y + shieldButtonRect_.h + 10);
        }

        if (paused_)
        {
            renderPauseMenu(renderer);
        }
    }

    void GameScene::updatePauseMenu()
    {
        float mouseX = input().getMouseX();
        float mouseY = input().getMouseY();

        bool hoverResume = mouseX >= resumeButtonRect_.x && mouseX <= resumeButtonRect_.x + resumeButtonRect_.w &&
                           mouseY >= resumeButtonRect_.y && mouseY <= resumeButtonRect_.y + resumeButtonRect_.h;
        bool hoverExit = mouseX >= exitPauseButtonRect_.x && mouseX <= exitPauseButtonRect_.x + exitPauseButtonRect_.w &&
                         mouseY >= exitPauseButtonRect_.y && mouseY <= exitPauseButtonRect_.y + exitPauseButtonRect_.h;

        if (input().isMousePressed(SDL_BUTTON_LEFT))
        {
            if (hoverResume)
            {
                paused_ = false;
            }
            else if (hoverExit)
            {
                paused_ = false;

                if (sceneManager_ != nullptr)
                {
                    sceneManager_->changeScene(std::make_unique<MenuScene>());
                }
            }
        }
    }
    void GameScene::renderPauseMenu(Renderer &renderer)
    {
        SDL_Texture *popup = TextureManager::instance().getTexture("endgame_popup");
        TTF_Font *font = FontManager::instance().getFont("menu");
        if (popup == nullptr || font == nullptr)
            return;

        const float popupWidth = 700.0f;
        const float popupHeight = popupWidth * 320.0f / 700.0f;
        const float popupX = (Constants::SCREEN_WIDTH - popupWidth) / 2.0f;
        const float popupY = (Constants::SCREEN_HEIGHT - popupHeight) / 2.0f;
        renderer.drawTexture(popup, popupX, popupY, popupWidth, popupHeight);
        const float sx = popupWidth / 700.0f;
        const float sy = popupHeight / 320.0f;

        SDL_Color white{255, 255, 255, 255};
        SDL_Color yellow{255, 255, 0, 255};
        SDL_Color orange{255, 165, 0, 255};
        renderer.drawTextCentered("What happened?", font, orange, Constants::SCREEN_WIDTH / 2, popupY + 65.0f * sy);
        constexpr float buttonWidth = 220.0f;
        constexpr float buttonHeight = 50.0f;
        constexpr float buttonY = 170.0f;

        resumeButtonRect_ = {popupX + 100.0f * sx, popupY + buttonY * sy, buttonWidth * sx, buttonHeight * sy};
        exitPauseButtonRect_ = {popupX + 380.0f * sx, popupY + buttonY * sy, buttonWidth * sx, buttonHeight * sy};
        const float mouseX = input().getMouseX();
        const float mouseY = input().getMouseY();

        const bool hoverResume = mouseX >= resumeButtonRect_.x && mouseX <= resumeButtonRect_.x + resumeButtonRect_.w &&
                                 mouseY >= resumeButtonRect_.y && mouseY <= resumeButtonRect_.y + resumeButtonRect_.h;
        const bool hoverExit = mouseX >= exitPauseButtonRect_.x && mouseX <= exitPauseButtonRect_.x + exitPauseButtonRect_.w &&
                               mouseY >= exitPauseButtonRect_.y && mouseY <= exitPauseButtonRect_.y + exitPauseButtonRect_.h;
        SDL_Color resumeColor = hoverResume ? yellow : white;
        std::string resumeText = hoverResume ? "> RESUME" : "   RESUME";

        renderer.drawTextCentered(resumeText, font, resumeColor, popupX + 205.0f * sx, popupY + 200.0f * sy);
        SDL_Color exitColor = hoverExit ? yellow : white;
        std::string exitText = hoverExit ? "> EXIT" : "  EXIT";
        renderer.drawTextCentered(exitText, font, exitColor, popupX + 486.0f * sx, popupY + 200.0f * sy);
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
        if (mouseX >= replayButtonRect_.x && mouseX <= replayButtonRect_.x + replayButtonRect_.w &&
            mouseY >= replayButtonRect_.y && mouseY <= replayButtonRect_.y + replayButtonRect_.h)
        {
            endMenuIndex_ = 0;
        }
        else if (mouseX >= menuButtonRect_.x && mouseX <= menuButtonRect_.x + menuButtonRect_.w &&
                 mouseY >= menuButtonRect_.y && mouseY <= menuButtonRect_.y + menuButtonRect_.h)
        {
            endMenuIndex_ = 1;
        }

        if (leftClick && !mousePressed)
        {

            if (mouseX >= replayButtonRect_.x && mouseX <= replayButtonRect_.x + replayButtonRect_.w &&
                mouseY >= replayButtonRect_.y && mouseY <= replayButtonRect_.y + replayButtonRect_.h)
            {
                endMenuIndex_ = 0;
                enter();
            }

            else if (mouseX >= menuButtonRect_.x && mouseX <= menuButtonRect_.x + menuButtonRect_.w &&
                     mouseY >= menuButtonRect_.y && mouseY <= menuButtonRect_.y + menuButtonRect_.h)
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
        const float popupX = (Constants::SCREEN_WIDTH - popupWidth) / 2.0f;
        const float popupY = (Constants::SCREEN_HEIGHT - popupHeight) / 2.0f;

        if (popup != nullptr)
        {
            renderer.drawTexture(popup, popupX, popupY, popupWidth, popupHeight);
        }
        const float sx = popupWidth / 700.0f;
        const float sy = popupHeight / 320.0f;

        constexpr float buttonWidth = 220.0f;
        constexpr float buttonHeight = 50.0f;
        constexpr float buttonY = 195.0f;

        replayButtonRect_ = {popupX + 100.0f * sx, popupY + buttonY * sy, buttonWidth * sx, buttonHeight * sy};
        menuButtonRect_ = {popupX + 380.0f * sx, popupY + buttonY * sy, buttonWidth * sx, buttonHeight * sy};

        TTF_Font *font = FontManager::instance().getFont("menu");
        if (font == nullptr)
        {
            return;
        }

        SDL_Color white{255, 255, 255, 255};
        SDL_Color yellow{255, 255, 0, 255};
        SDL_Color red{255, 80, 80, 255};

        renderer.drawTextCentered(playerWon_ ? "YOU WIN" : "GAME OVER", font, red, static_cast<int>(popupX + 350.0f * sx), popupY + 55.0f * sy);
        renderer.drawTextCentered("PLAYER : " + playerName_, font, white, Constants::SCREEN_WIDTH / 2, popupY + 100.0f * sy);
        renderer.drawTextCentered("SCORE : " + std::to_string(score_), font, white, Constants::SCREEN_WIDTH / 2, popupY + 150.0f * sy);
        SDL_Color replayColor = (endMenuIndex_ == 0) ? yellow : white;
        std::string replayText = (endMenuIndex_ == 0) ? "> REPLAY" : "REPLAY";
        renderer.drawTextCentered(replayText, font, replayColor, popupX + 210.0f * sx, popupY + 215.0f * sy);
        SDL_Color menuColor = (endMenuIndex_ == 1) ? yellow : white;
        std::string menuText = (endMenuIndex_ == 1) ? "> MENU" : "MENU";
        renderer.drawTextCentered(menuText, font, menuColor, popupX + 490.0f * sx, popupY + 215.0f * sy);
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

    SDL_FRect GameScene::getPlayerHitbox() const
    {
        const float playerSpriteWidth = 48.0f;
        const float playerSpriteHeight = 48.0f;
        const float playerHitboxWidth = 40.0f;
        const float playerHitboxHeight = 40.0f;

        const float playerHitboxX = player_.x + (playerSpriteWidth - playerHitboxWidth) / 2.0f;
        const float playerHitboxY = player_.y + (playerSpriteHeight - playerHitboxHeight) / 2.0f;

        return {playerHitboxX, playerHitboxY, playerHitboxWidth, playerHitboxHeight};
    }

    SDL_FRect GameScene::getPlayerShieldHitbox() const
    {
        const float shieldSize = 80.0f;
        const float playerSize = 48.0f; // Kích thước sprite của player
        const float shieldX = player_.x + (playerSize - shieldSize) / 2.0f;
        const float shieldY = player_.y + (playerSize - shieldSize) / 2.0f;
        return {shieldX, shieldY, shieldSize, shieldSize};
    }

    SDL_FRect GameScene::getEnemyLaserHitbox(const Bullet &bullet) const
    {
        // Các tính toán này dựa trên texture 'enemy_laser'.
        // Texture gốc có kích thước 64x64, nhưng thực thể đạn có thể được co giãn.
        const float baseTextureWidth = 64.0f;
        const float baseTextureHeight = 64.0f;
        const float scaleX = bullet.width / baseTextureWidth;
        const float scaleY = bullet.height / baseTextureHeight;

        // Phần tia sáng thực tế bên trong texture
        const float hitboxOriginX = 11.0f;    // Tọa độ pixel trong texture
        const float hitboxOriginY = 16.0f;    // Tọa độ pixel trong texture
        const float hitboxBaseWidth = 8.0f;   // Kích thước pixel trong texture
        const float hitboxBaseHeight = 10.0f; // Kích thước pixel trong texture

        const float finalHitboxX = bullet.x + (hitboxOriginX * scaleX);
        const float finalHitboxY = bullet.y + (hitboxOriginY * scaleY);
        const float finalHitboxWidth = hitboxBaseWidth * scaleX;
        const float finalHitboxHeight = hitboxBaseHeight * scaleY;

        return {finalHitboxX, finalHitboxY, finalHitboxWidth, finalHitboxHeight};
    }
} // namespace SpaceInvaders