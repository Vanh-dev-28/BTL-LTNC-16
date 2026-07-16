#include "Scenes/MenuScene.h"
#include "Core/Renderer.h"
#include "Scenes/GameScene.h"
#include "Scenes/RankingScene.h"
#include <memory>
#include <iostream>
#include "Managers/SceneManager.h"
#include "Managers/FontManager.h"
#include <SDL3/SDL.h>

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

            if (selectedIndex_ >= menuItems_.size())
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
            SDL_Log("Settings");
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
    TTF_Font* font = FontManager::instance().getFont("menu");

    if (font == nullptr)
        return;

    SDL_Color white{255,255,255,255};
    SDL_Color yellow{255,255,0,255};

    renderer.drawText(
        "SPACE INVADERS",
        font,
        white,
        180,
        80);
    int startY = 220;

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

    renderer.drawText(
        text,
        font,
        color,
        240,
        startY + static_cast<int>(i) * 60);
}
}

}