#include "Scenes/MenuScene.h"
#include "Core/Renderer.h"
#include "Scenes/GameScene.h"
#include "Scenes/RankingScene.h"
#include <memory>
#include <iostream>
#include "Managers/SceneManager.h"
#include "Managers/FontManager.h"
#include "Managers/TextureManager.h"
#include "Managers/AudioManager.h"
#include "Utils/Constants.h"
#include <SDL3/SDL.h>
#include "Scenes/SettingsScene.h"
#include "Core/Input.h"

namespace SpaceInvaders
{

void MenuScene::enter()
{
    AudioManager::instance().playMusic("../assets/audio/music/background_music.mp3");
}

void MenuScene::exit()
{
}

void MenuScene::update(float)
{
    const float mouseX = input().getMouseX();
    const float mouseY = input().getMouseY();

    const int menuStartY = 260;
    const int menuSpacing = 70;

    const int itemWidth = 400;
    const int itemHeight = 55;

    const int centerX = Constants::SCREEN_WIDTH / 2;

    // Xác định item đang được hover
    for (size_t i = 0; i < menuItems_.size(); ++i)
    {
        const int itemY =
            menuStartY + static_cast<int>(i) * menuSpacing;

        const int left =
            centerX - itemWidth / 2;

        const int right =
            centerX + itemWidth / 2;

        const int top =
            itemY - itemHeight / 2;

        const int bottom =
            itemY + itemHeight / 2;

        const bool mouseInside =
            mouseX >= left &&
            mouseX <= right &&
            mouseY >= top &&
            mouseY <= bottom;

        if (!mouseInside)
        {
            continue;
        }

        // Hover
        selectedIndex_ = static_cast<int>(i);

        // Click chuột trái
        if (input().isMousePressed(SDL_BUTTON_LEFT))
        {
            switch (selectedIndex_)
            {
            case 0:
                // START GAME
                SDL_Log("Start Game");

                if (sceneManager_ != nullptr)
                {
                    sceneManager_->changeScene(
                        std::make_unique<GameScene>());
                }
                break;

            case 1:
                // RANKING
                if (sceneManager_ != nullptr)
                {
                    sceneManager_->changeScene(
                        std::make_unique<RankingScene>());
                }
                break;

            case 2:
                // SETTINGS
                if (sceneManager_ != nullptr)
                {
                    sceneManager_->changeScene(
                        std::make_unique<SettingsScene>());
                }
                break;

            case 3:
                // EXIT
                {
                    SDL_Event quitEvent{};
                    quitEvent.type = SDL_EVENT_QUIT;
                    SDL_PushEvent(&quitEvent);
                }
                break;
            }
        }

        break;
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