#include "Scenes/MenuScene.h"
#include "Core/Renderer.h"
#include "Scenes/GameScene.h"
#include "Scenes/RankingScene.h"
#include <memory>
#include <iostream>
#include "Managers/SceneManager.h"
#include "Managers/FontManager.h"
#include "Managers/TextureManager.h"
#include "Utils/Constants.h"
#include <SDL3/SDL.h>
#include "Scenes/SettingsScene.h"

namespace SpaceInvaders
{

void MenuScene::enter()
{
}

void MenuScene::exit()
{
}

void MenuScene::update(float)
{
    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    static bool upPressed = false;
    static bool downPressed = false;
    static bool enterPressed = false;

    if (keyboard[SDL_SCANCODE_UP])
    {
        if (!upPressed)
        {
            selectedIndex_--;

            if (selectedIndex_ < 0)
                selectedIndex_ = menuItems_.size() - 1;
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

            if (selectedIndex_ >= static_cast<int>(menuItems_.size()))
                selectedIndex_ = 0;
        }

        downPressed = true;
    }
    else
    {
        downPressed = false;
    }

    if (keyboard[SDL_SCANCODE_RETURN])
{
    if (!enterPressed)
    {
        switch (selectedIndex_)
        {
        case 0:
            SDL_Log("Start Game");

            if (sceneManager_ != nullptr)
    {
        sceneManager_->changeScene(std::make_unique<GameScene>());
    }
            break;

        case 1:
            sceneManager_->changeScene(
        std::make_unique<RankingScene>());
            break;

        case 2:
            sceneManager_->changeScene(
        std::make_unique<SettingsScene>());
            break;

        case 3:
            SDL_Log("Exit");
            break;
        }
    }

    enterPressed = true;
}
else
{
    enterPressed = false;
}
}

void MenuScene::render(Renderer& renderer)
{
    SDL_Texture* background =
    TextureManager::instance().getTexture("menu_background");

    if (background != nullptr)
    {
        renderer.drawTexture(
        background,
        0.0f,
        0.0f,
        static_cast<float>(Constants::SCREEN_WIDTH),
        static_cast<float>(Constants::SCREEN_HEIGHT));
    }
    TTF_Font* titleFont =
    FontManager::instance().getFont("menu_title");

    TTF_Font* menuFont =
    FontManager::instance().getFont("menu");

    if (titleFont == nullptr || menuFont == nullptr)
        return;

    SDL_Color white{255,255,255,255};
    SDL_Color yellow{255,255,0,255};

    const int titleY = 70;

    const int menuStartY = 260;
    const int menuSpacing = 70;

    renderer.drawTextCentered(
    "SPACE INVADERS",
    titleFont,
    white,
    Constants::SCREEN_WIDTH / 2,
    titleY);

for (size_t i = 0; i < menuItems_.size(); i++)
{
    SDL_Color color =
        (i == static_cast<size_t>(selectedIndex_))
            ? yellow
            : white;

    std::string text = menuItems_[i];

    if (i == static_cast<size_t>(selectedIndex_))
    {
        text = "> " + text;
    }

    renderer.drawTextCentered(
    text,
    menuFont,
    color,
    Constants::SCREEN_WIDTH / 2,
    menuStartY + static_cast<int>(i) * menuSpacing);

}
}
} // namespace SpaceInvaders