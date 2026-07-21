#include "Scenes/RankingScene.h"

#include "Core/Renderer.h"
#include "Managers/FontManager.h"
#include "Managers/SceneManager.h"
#include "Scenes/MenuScene.h"
#include "Managers/RankingManager.h"

#include <SDL3/SDL.h>
#include <memory>
#include <fstream>

namespace SpaceInvaders
{

void RankingScene::enter()
{
    RankingManager::instance().load("assets/data/ranking.txt");
}

void RankingScene::exit()
{
}

void RankingScene::update(float)
{
    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    static bool escPressed = false;

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
}

void RankingScene::render(Renderer& renderer)
{
    TTF_Font* font = FontManager::instance().getFont("menu");

    if (font == nullptr)
        return;

    SDL_Color white{255,255,255,255};

    renderer.drawText("RANKING", font, white, 220, 60);

    const auto& scores = RankingManager::instance().getScores();

    int startY = 170;

    for (size_t i = 0; i < scores.size(); ++i) {
    std::string text =
        std::to_string(i + 1) + ". " +
        scores[i].name + "    " +
        std::to_string(scores[i].score);

    renderer.drawText(
        text,
        font,
        white,
        180,
        startY + static_cast<int>(i) * 60);
     }

    renderer.drawText("ESC : Back", font, white, 15, 15);
}

}