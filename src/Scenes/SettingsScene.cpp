#include "Scenes/SettingsScene.h"

#include "Core/Renderer.h"
#include "Managers/FontManager.h"
#include "Managers/SceneManager.h"
#include "Managers/SettingsManager.h"
#include "Scenes/MenuScene.h"
#include "Utils/Constants.h"
#include "Core/Input.h"

#include <SDL3/SDL.h>
#include <memory>

namespace SpaceInvaders
{

void SettingsScene::enter()
{
}

void SettingsScene::exit()
{
}

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
    if (input().isKeyPressed(SDL_SCANCODE_UP))
    {
        selectedIndex_--;

        if (selectedIndex_ < 0)
        {
        selectedIndex_ =
            static_cast<int>(settingItems_.size()) - 1;
        }
    }

    if (input().isKeyPressed(SDL_SCANCODE_DOWN))
    {
        selectedIndex_++;

        if (selectedIndex_ >= static_cast<int>(settingItems_.size()))
        {
            selectedIndex_ = 0;
        }
    }

    if (input().isKeyPressed(SDL_SCANCODE_RIGHT))
    {
        switch (selectedIndex_)
        {
        case 0: 
            state_ = SettingsState::ResolutionPopup;
            break;

        case 1:     // Music 
        {
            int volume = SettingsManager::instance().getMusicVolume();
            volume = std::min(volume + 10, 100);
            SettingsManager::instance().setMusicVolume(volume);
            break;
        }

        case 2:     // SFX 
        {
            int volume = SettingsManager::instance().getSFXVolume();
            volume = std::min(volume + 10, 100);
            SettingsManager::instance().setSFXVolume(volume);
            break;
        }

        default:
            break;
        }
    }

    if (input().isKeyPressed(SDL_SCANCODE_LEFT))
    {
        switch (selectedIndex_)
        {
        case 1:
        {
            int volume = SettingsManager::instance().getMusicVolume();
            volume = std::max(volume - 10, 0);
            SettingsManager::instance().setMusicVolume(volume);
            break;
        }

        case 2:
        {
            int volume = SettingsManager::instance().getSFXVolume();
            volume = std::max(volume - 10, 0);
            SettingsManager::instance().setSFXVolume(volume);
            break;
        }

        default:
            break;
        }
    }
    if (input().isKeyPressed(SDL_SCANCODE_RETURN))
    {
        switch (selectedIndex_)
        {
        case 3:
            SettingsManager::instance().apply();
            sceneManager_->changeScene(std::make_unique<MenuScene>());
            break;
        case 4:
            SettingsManager::instance().discard();
            sceneManager_->changeScene(std::make_unique<MenuScene>());
        break;

        default:
            break;
        }
    }
}
void SettingsScene::render(Renderer& renderer)
{
    TTF_Font* font = FontManager::instance().getFont("menu");

    if (font == nullptr)
        return;

    SDL_Color white{255,255,255,255};

    renderer.drawTextCentered(
        "SETTINGS",
        font,
        white,
        640,
        80);

    const int menuStartY = 180;
    const int menuSpacing = 65;

    SDL_Color yellow{255,255,0,255};

    for (size_t i = 0; i < settingItems_.size(); i++)
    {
    SDL_Color color =
        (i == static_cast<size_t>(selectedIndex_))
            ? yellow
            : white;

    std::string text = settingItems_[i];
    SettingsManager& settings = SettingsManager::instance();
    switch (i)
    {
case 0:     // Resolution
    text += " : " + settings.getResolution();
    break;

case 1:     // Music
    text += " : " +
        std::to_string(settings.getMusicVolume()) + "%";
    break;

case 2:     // SFX
    text += " : " +
        std::to_string(settings.getSFXVolume()) + "%";
    break;
case 3:     // Apply
    break;

case 4:     // Discard
    break;
    }
    

    if (i == static_cast<size_t>(selectedIndex_))
    {
        text = "-> " + text;
    }

    renderer.drawTextCentered(
        text,
        font,
        color,
        Constants::SCREEN_WIDTH / 2,
        menuStartY + static_cast<int>(i) * menuSpacing);
    }

    renderer.drawTextCentered(
        "Apply to save - Discard to cancel",
        font,
        white,
        640,
        620);

if (state_ == SettingsState::ResolutionPopup)
{
    renderResolutionPopup(renderer);
}
}

void SettingsScene::updateResolutionPopup()
{
    if (input().isKeyPressed(SDL_SCANCODE_LEFT))
    {
        state_ = SettingsState::Normal;
    }

    if (input().isKeyPressed(SDL_SCANCODE_UP))
    {
        resolutionPopupIndex_--;

        if (resolutionPopupIndex_ < 0)
        {
            resolutionPopupIndex_ =
            static_cast<int>(resolutionOptions_.size()) - 1;
        }
    }

    if (input().isKeyPressed(SDL_SCANCODE_DOWN))
    {
        resolutionPopupIndex_++;

        if (resolutionPopupIndex_ >=
            static_cast<int>(resolutionOptions_.size()))
        {
            resolutionPopupIndex_ = 0;
        }
    }

    if (input().isKeyPressed(SDL_SCANCODE_RETURN))
    {
        SettingsManager::instance().setResolution(
            resolutionOptions_[resolutionPopupIndex_]);

        state_ = SettingsState::Normal;
    }
}

void SettingsScene::renderResolutionPopup(Renderer& renderer)
{
    // ===== Popup Layout =====
    const float popupWidth = 700.0f;
    const float popupHeight = 320.0f;

    const float popupX =
        (Constants::SCREEN_WIDTH - popupWidth) / 2.0f;
    const float popupY = 170.0f;

    const float titleY = popupY + 18.0f;
    const float separatorY = popupY + 60.0f;

    const float listX = popupX + 70.0f;
    const float listStartY = popupY + 95.0f;
    const float itemSpacing = 55.0f;

    // ===== Colors =====
    SDL_Color background{25, 25, 35, 240};
    SDL_Color border{255, 255, 255, 255};
    SDL_Color white{255, 255, 255, 255};
    SDL_Color yellow{255, 255, 0, 255};

    // ===== Font =====
    TTF_Font* font = FontManager::instance().getFont("menu");

    if (font == nullptr)
        return;

    // ===== Popup =====
    renderer.fillRect(
        popupX,
        popupY,
        popupWidth,
        popupHeight,
        background);

    renderer.drawRect(
        popupX,
        popupY,
        popupWidth,
        popupHeight,
        border);

    // ===== Title =====
    renderer.drawTextCentered(
        "RESOLUTION",
        font,
        white,
        Constants::SCREEN_WIDTH / 2,
        static_cast<int>(titleY));

    renderer.drawLine(
        popupX,
        separatorY,
        popupX + popupWidth,
        separatorY,
        white);

    // ===== Resolution List =====
    for (size_t i = 0; i < resolutionOptions_.size(); i++)
    {
        SDL_Color color =
            (i == static_cast<size_t>(resolutionPopupIndex_))
                ? yellow
                : white;

        std::string text = resolutionOptions_[i];

        if (i == static_cast<size_t>(resolutionPopupIndex_))
        {
            text = "->  " + text;
        }

        renderer.drawText(
            text,
            font,
            color,
            static_cast<int>(listX),
            static_cast<int>(listStartY + i * itemSpacing));
    }
}

}