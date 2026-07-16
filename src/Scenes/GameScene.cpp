#include "Scenes/GameScene.h"
#include "Core/Renderer.h"

namespace SpaceInvaders
{

void GameScene::enter()
{
}

void GameScene::exit()
{
}

void GameScene::update(float)
{
}

void GameScene::render(Renderer& renderer)
{
    // Tạm thời tô nền màu xanh để biết đã chuyển scene
    SDL_SetRenderDrawColor(
        renderer.getSDLRenderer(),
        0,
        120,
        255,
        255);

    SDL_RenderClear(renderer.getSDLRenderer());
}

}