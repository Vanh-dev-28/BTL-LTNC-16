#include "Scenes/SettingsScene.h"

#include "Core/Renderer.h"
#include "Managers/FontManager.h"
#include "Managers/SceneManager.h"
#include "Managers/SettingsManager.h"
#include "Scenes/MenuScene.h"
#include "Utils/Constants.h"

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
    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    static bool upPressed = false;
    static bool downPressed = false;
    static bool escPressed = false;
    static bool rightPressed = false;

    if (keyboard[SDL_SCANCODE_UP])
    {
        if (!upPressed)
        {
            selectedIndex_--;

            if (selectedIndex_ < 0)
            {
                selectedIndex_ =
                    static_cast<int>(settingItems_.size()) - 1;
            }
        }

        upPressed = true;
    }
    else
    {
        upPressed = false;
    }

    if (keyboard[SDL_SCANCODE_DOWN])
    {
        if (!downPressed)
        {
            selectedIndex_++;

            if (selectedIndex_ >= static_cast<int>(settingItems_.size()))
            {
                selectedIndex_ = 0;
            }
        }

        downPressed = true;
    }
    else
    {
        downPressed = false;
    }

    if (keyboard[SDL_SCANCODE_ESCAPE])
    {
        if (!escPressed)
        {
            sceneManager_->changeScene(std::make_unique<MenuScene>());
        }

        escPressed = true;
    }
    else
    {
        escPressed = false;
    }

    if (keyboard[SDL_SCANCODE_RIGHT])
    {  
        if (!rightPressed)
       {
        if (selectedIndex_ == 0)
        {
            state_ = SettingsState::ResolutionPopup;
        }
    }

    rightPressed = true;
    }
    else
    {
    rightPressed = false;
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
case 0:
    text += " : " + settings.getResolution();
    break;

case 1:
    text += settings.isFullscreen()
        ? " : ON"
        : " : OFF";
    break;

case 2:
    text += " : " +
        std::to_string(settings.getMusicVolume()) + "%";
    break;

case 3:
    text += " : " +
        std::to_string(settings.getSFXVolume()) + "%";
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
        "ESC : Back",
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
    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    static bool leftPressed = false;

    if (keyboard[SDL_SCANCODE_LEFT])
    {
       if (!leftPressed)
       {
        state_ = SettingsState::Normal;
       }

    leftPressed = true;
    }
    else
    {
    leftPressed = false;
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