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
    void MenuScene::exit(){ }

    SDL_FRect MenuScene::getMenuItemRect(int index) const
    {
        const float centerX = Constants::SCREEN_WIDTH / 2.0f;
        const float menuStartY = 260.0f;
        const float menuSpacing = 70.0f;
        const float width = 500.0f;
        const float height = 65.0f;

        const float x = centerX - width / 2.0f;
        const float y = menuStartY + static_cast<float>(index) * menuSpacing - height / 2.0f;

        return SDL_FRect{x, y, width, height};
    }

    void MenuScene::update(float)
    {
        const float mouseX = input().getMouseX();
        const float mouseY = input().getMouseY();

        for (size_t i = 0; i < menuItems_.size(); ++i)
        {
            SDL_FRect rect = getMenuItemRect(static_cast<int>(i));
            const bool mouseInside = mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y && mouseY <= rect.y + rect.h;
            if (!mouseInside)
                continue;
            selectedIndex_ = static_cast<int>(i);
            if (input().isMousePressed(SDL_BUTTON_LEFT))
            {
                switch (selectedIndex_)
                {
                case 0:
                    if (sceneManager_ != nullptr)
                    {
                        sceneManager_->changeScene(std::make_unique<GameScene>(""));
                    }
                    return;
                case 1:
                    if (sceneManager_ != nullptr)
                    {
                        sceneManager_->changeScene(std::make_unique<RankingScene>());
                    }
                    return;
                case 2:
                    if (sceneManager_ != nullptr)
                    {
                        sceneManager_->changeScene(std::make_unique<SettingsScene>());
                    }
                    return;
                case 3:
                    {
                        SDL_Event quitEvent{};
                        quitEvent.type = SDL_EVENT_QUIT;
                        SDL_PushEvent(&quitEvent);
                        return;
                    }
                }
            }
            break;
        }
    }

    void MenuScene::render(Renderer& renderer)
    {
        SDL_Texture* background = TextureManager::instance().getTexture("menu_background");
        if (background != nullptr)
        {
            renderer.drawTexture(background, 0.0f, 0.0f, static_cast<float>(Constants::SCREEN_WIDTH), static_cast<float>(Constants::SCREEN_HEIGHT));
        }
    
        TTF_Font* titleFont = FontManager::instance().getFont("menu_title");
        TTF_Font* menuFont = FontManager::instance().getFont("menu");

        if (titleFont == nullptr || menuFont == nullptr)
            return;

        SDL_Color white{255,255,255,255};
        SDL_Color yellow{255,255,0,255};

        const int titleY = 70;
        const int menuStartY = 260;
        const int menuSpacing = 70;
        renderer.drawTextCentered("SPACE INVADERS", titleFont, white, Constants::SCREEN_WIDTH / 2, titleY);

        for (size_t i = 0; i < menuItems_.size(); i++)  
        {
            SDL_Color color = (i == static_cast<size_t>(selectedIndex_)) ? yellow : white;
            std::string text = menuItems_[i];
            if (i == static_cast<size_t>(selectedIndex_))
            {
                text = "> " + text;
            }
            renderer.drawTextCentered(text, menuFont, color, Constants::SCREEN_WIDTH / 2, menuStartY + static_cast<int>(i) * menuSpacing);
        }
    }
}