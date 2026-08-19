#include "Core/Renderer.h"
#include "Core/Input.h"
#include "Managers/FontManager.h"
#include "Managers/SceneManager.h"
#include "Managers/RankingManager.h"
#include "Managers/TextureManager.h"

#include "Scenes/RankingScene.h"
#include "Scenes/MenuScene.h"
#include "Utils/Constants.h"

#include <SDL3/SDL.h>
#include <memory>
#include <algorithm>

namespace SpaceInvaders
{
    void RankingScene::enter()
    {
        RankingManager::instance().load("../assets/data/ranking.txt");
    }
    void RankingScene::exit(){ }

    void RankingScene::update(float)
    {
        if (input().isKeyPressed(SDL_SCANCODE_ESCAPE))
        {
            if (sceneManager_ != nullptr)
            {
                sceneManager_->changeScene(std::make_unique<MenuScene>());
            }
        }
    }

    void RankingScene::render(Renderer& renderer)
    {
        SDL_Texture* background = TextureManager::instance().getTexture("ranking_background");
        if (background != nullptr)
        {
            renderer.drawTexture(background, 0.0f, 0.0f, static_cast<float>(Constants::SCREEN_WIDTH), static_cast<float>(Constants::SCREEN_HEIGHT));
        }
        else
        {
            SDL_SetRenderDrawColor(renderer.getSDLRenderer(), 10, 20, 40, 255);
            SDL_RenderClear(renderer.getSDLRenderer());
        }
        TTF_Font* titleFont = FontManager::instance().getFont("menu_title");
        TTF_Font* font = FontManager::instance().getFont("menu");
        if (titleFont == nullptr || font == nullptr) 
            return;

        SDL_Color white{255, 255, 255, 255};
        SDL_Color yellow{255, 255, 0, 255};
        renderer.drawTextCentered("TOP 10 BEST PLAYERS", titleFont, yellow, Constants::SCREEN_WIDTH / 2, 80);
        renderer.drawText("STT", font, white, 250, 180);
        renderer.drawText("NAME", font, white, 400, 180);
        renderer.drawText("SCORE", font, white, 800,180);

        const auto& scores = RankingManager::instance().getScores();
        const size_t maxPlayers = std::min<size_t>(10, scores.size());
        const int startY = 230;
        const int spacing = 50;

        for (size_t i = 0; i < maxPlayers; ++i)
        {
            const auto& entry = scores[i];
            const int y = startY + static_cast<int>(i) * spacing;
            renderer.drawText(std::to_string(i + 1), font, white, 250, y);
            renderer.drawText(entry.name, font, white, 400, y);
            renderer.drawText(std::to_string(entry.score), font, white, 800, y);
        }

        renderer.drawTextCentered("ESC : BACK", font, white, Constants::SCREEN_WIDTH / 2, Constants::SCREEN_HEIGHT - 50);
    }
}