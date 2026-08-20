#include "Scenes/SettingsScene.h"
#include "Core/Renderer.h"
#include "Managers/FontManager.h"
#include "Managers/SceneManager.h"
#include "Managers/SettingsManager.h"
#include "Scenes/MenuScene.h"
#include "Utils/Constants.h"
#include "Core/Input.h"
#include "Managers/TextureManager.h"
#include <SDL3/SDL.h>
#include <memory>
#include <algorithm>

namespace SpaceInvaders
{
    void SettingsScene::enter(){ }
    void SettingsScene::exit() { }

    void SettingsScene::update(float)
    {
        switch (state_)
        {
        case SettingsState::Normal:
            updateNormal();
            break;

        case SettingsState::ResolutionPopup:
            updateResolutionPopup();
            break;
        }  
    }

    void SettingsScene::updateNormal()
    {
        const float mouseX = input().getMouseX();
        const float mouseY = input().getMouseY();
        const int menuStartY = 180;
        const int menuSpacing = 65;
        const int left = 250;
        const int right = 1030;

        for (size_t i = 0; i < settingItems_.size(); ++i)
        {
            const int itemY = menuStartY + static_cast<int>(i) * menuSpacing;
            const int top = itemY - 30;
            const int bottom = itemY + 30;
            
            if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom)
            {
                selectedIndex_ = static_cast<int>(i);
                break;
            }
        }

        if (selectedIndex_ == 0 && input().isMousePressed(SDL_BUTTON_LEFT))
        {
            const int itemY = menuStartY;

            if (mouseY >= itemY - 30 && mouseY <= itemY + 30)
            {
                state_ = SettingsState::ResolutionPopup;
            }
        }

        if (isMouseOverVolumeSlider(1, mouseX, mouseY))
        {
            selectedIndex_ = 1;
            if (input().isMouseDown(SDL_BUTTON_LEFT))
            {
                updateVolumeSlider(1, mouseX, mouseY);
            }
        }

        if (selectedIndex_ == 2 && input().isMousePressed(SDL_BUTTON_LEFT))
        {
            const int itemY = menuStartY + 2 * menuSpacing;

            if (mouseY >= itemY - 30 && mouseY <= itemY + 30)
            {
                SettingsManager::instance().apply();
                if (sceneManager_ != nullptr)
                {
                    sceneManager_->changeScene(std::make_unique<MenuScene>());
                }
            }
        }

        if (selectedIndex_ == 3 && input().isMousePressed(SDL_BUTTON_LEFT))
        {
            const int itemY = menuStartY + 3 * menuSpacing;

            if (mouseY >= itemY - 30 && mouseY <= itemY + 30)
            {
                SettingsManager::instance().discard();

                if (sceneManager_ != nullptr)
                {
                    sceneManager_->changeScene(std::make_unique<MenuScene>());
                }   
            }
        }
    }
    
    void SettingsScene::updateResolutionPopup()
    {
        const float mouseX = input().getMouseX();
        const float mouseY = input().getMouseY();  
        const float popupWidth = 700.0f;
        const float popupHeight = 320.0f;
        const float popupX = (Constants::SCREEN_WIDTH - popupWidth) / 2.0f;
        const float popupY = 170.0f;
        const float listX = popupX + 70.0f;
        const float listStartY = popupY + 95.0f;
        const float itemSpacing = 55.0f;

        if (input().isMousePressed(SDL_BUTTON_LEFT))
        {
            if (mouseX < popupX || mouseX > popupX + popupWidth || mouseY < popupY || mouseY > popupY + popupHeight)
            {
                state_ = SettingsState::Normal;
                return;
            }
        }

        for (size_t i = 0; i < resolutionOptions_.size(); ++i)
        {
            const float itemY = listStartY + static_cast<float>(i) * itemSpacing;
            const float left = popupX + 40.0f;
            const float right = popupX + popupWidth - 40.0f;
            const float top = itemY - 25.0f;
            const float bottom = itemY + 25.0f;
            if (mouseX >= left && mouseX <= right &&
                mouseY >= top && mouseY <= bottom)
            {
                resolutionPopupIndex_ = static_cast<int>(i);
                if (input().isMousePressed(SDL_BUTTON_LEFT))
                {
                    SettingsManager::instance().setResolution(resolutionOptions_[i]);
                    state_ = SettingsState::Normal;
                }
                break;
            }
        }
    }

    bool SettingsScene::isMouseOverVolumeSlider(int index, float mouseX, float mouseY) const
    {
        if (index != 1)
            return false;

        const float barX = 500.0f;
        const float barWidth = 350.0f;
        const float barHeight = 24.0f;
        const int menuStartY = 180;
        const int menuSpacing = 65;
        const float barY = static_cast<float>(menuStartY + index * menuSpacing + 8);
        const float hitPadding = 12.0f;

        return mouseX >= barX && mouseX <= barX + barWidth && mouseY >= barY - hitPadding && mouseY <= barY + barHeight + hitPadding;
    }
    void SettingsScene::updateVolumeSlider(int index, float mouseX, float mouseY)
    {
        if (index != 1)
            return;
        const float barX = 500.0f;
        const float barWidth = 350.0f;
        const int menuStartY = 180;
        const int menuSpacing = 65;
        const float barY = static_cast<float>(menuStartY + index * menuSpacing + 8);
        const float barHeight = 24.0f;
        const float hitPadding = 12.0f;

        if (mouseX < barX || mouseX > barX + barWidth ||
            mouseY < barY - hitPadding || mouseY > barY + barHeight + hitPadding)
        {
            return;
        }
        float relativeX = mouseX - barX;
        float percentage = relativeX / barWidth;
        percentage = std::clamp(percentage, 0.0f, 1.0f);
        int volume = static_cast<int>(percentage * 100.0f + 0.5f);
        SettingsManager::instance().setMusicVolume(volume);
    }

    void SettingsScene::render(Renderer& renderer)
    {
        SDL_Texture* background = TextureManager::instance().getTexture("settings_background");
        if (background != nullptr)
        {
            renderer.drawTexture(background, 0.0f, 0.0f, static_cast<float>(Constants::SCREEN_WIDTH), 
                                                        static_cast<float>(Constants::SCREEN_HEIGHT));
        }

        TTF_Font* font = FontManager::instance().getFont("menu");
        if (font == nullptr)
            return;

        SDL_Color white{255,255,255,255};
        SDL_Color yellow{255,255,0,255};
        renderer.drawTextCentered("SETTINGS", font, white, 640, 80);
        const int menuStartY = 180;
        const int menuSpacing = 65;
        for (size_t i = 0; i < settingItems_.size(); ++i)
        {
            const bool selected = i == static_cast<size_t>(selectedIndex_);
            if (i == 1)
            {
                drawVolumeBar(renderer, "Music", SettingsManager::instance().getMusicVolume(),
                                menuStartY + static_cast<int>(i) * menuSpacing, selected);
                continue;
            }

            SDL_Color color = selected ? SDL_Color{255, 255, 0, 255} : white;
            std::string text = settingItems_[i];
            if (i == 0)
            {
                text += " : " + SettingsManager::instance().getResolution();
            }

            if (selected)
            {
                text = "-> " + text;
            }
            renderer.drawTextCentered(text, font, color, Constants::SCREEN_WIDTH / 2,
                                    menuStartY + static_cast<int>(i) * menuSpacing);
        }
        renderer.drawTextCentered("Apply to save - Discard to cancel", font, white, Constants::SCREEN_WIDTH / 2, 620);

        if (state_ == SettingsState::ResolutionPopup)
        {
            renderResolutionPopup(renderer);
        }
    }

    void SettingsScene::renderResolutionPopup(Renderer& renderer)
    {
        const float popupWidth = 700.0f;
        const float popupHeight = 320.0f;
        const float popupX = (Constants::SCREEN_WIDTH - popupWidth) / 2.0f;
        const float popupY = 170.0f;
        const float titleY = popupY + 18.0f;
        const float separatorY = popupY + 60.0f;
        const float listX = popupX + 70.0f;
        const float listStartY = popupY + 95.0f;
        const float itemSpacing = 55.0f;
        SDL_Color white{255, 255, 255, 255};
        SDL_Color yellow{255, 255, 0, 255};
        TTF_Font* font = FontManager::instance().getFont("menu");
        if (font == nullptr)
            return;
        SDL_Texture* popup = TextureManager::instance().getTexture("resolution_popup");

        if (popup != nullptr)
        {
            renderer.drawTexture(popup, popupX, popupY, popupWidth, popupHeight);
        }
        renderer.drawTextCentered("RESOLUTION", font, white, Constants::SCREEN_WIDTH / 2, static_cast<int>(titleY));
        renderer.drawLine(popupX, separatorY, popupX + popupWidth, separatorY, white);
        for (size_t i = 0; i < resolutionOptions_.size(); i++)
        {
            SDL_Color color = (i == static_cast<size_t>(resolutionPopupIndex_))
                                ? yellow : white;
            std::string text = resolutionOptions_[i];

            if (i == static_cast<size_t>(resolutionPopupIndex_))
            {
                text = "->  " + text;
            }
            renderer.drawText(text, font, color, static_cast<int>(listX), static_cast<int>(listStartY + i * itemSpacing));
        }
    }

    void SettingsScene::drawVolumeBar(Renderer& renderer, const std::string& label, int volume, int y, bool selected)
    {
        TTF_Font* font = FontManager::instance().getFont("menu");
        if (font == nullptr)
            return;

        SDL_Color white{255, 255, 255, 255};
        SDL_Color yellow{255, 255, 0, 255};
        SDL_Color dark{60, 60, 60, 255};
        SDL_Color color = selected ? yellow : white;

        renderer.drawText(label, font, color, 300, y);
        const float barX = 500.0f;
        const float barY = static_cast<float>(y + 8);
        const float barWidth = 350.0f;
        const float barHeight = 24.0f;

        renderer.fillRect(barX, barY, barWidth, barHeight, dark);
        const float filledWidth = barWidth * static_cast<float>(volume) / 100.0f;
        
        renderer.fillRect(barX, barY, filledWidth, barHeight, color);
        renderer.drawRect(barX, barY, barWidth, barHeight, white);
        renderer.drawText(std::to_string(volume) + "%", font, color, static_cast<int>(barX + barWidth + 20), y);
    }
}