#include "Core/Game.h"

#include "Managers/AudioManager.h"
#include "Managers/FontManager.h"
#include "Managers/TextureManager.h"
#include "Utils/Constants.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

namespace SpaceInvaders {

Game::~Game()
{
    clean();
}

bool Game::initialize()
{
    if (initialized_) {
        return true;
    }
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0 ||
        (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG || TTF_Init() != 0 ||
        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) != 0 ||
        !window_.create("Space Invaders", Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT) ||
        !renderer_.create(window_.getSDLWindow())) {
        clean();
        return false;
    }

    initialized_ = true;
    running_ = true;
    return true;
}

void Game::run()
{
    while (running_) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            running_ = false;
        }
    }
    input_.update();
}

void Game::update()
{
    timer_.update();
    sceneManager_.update(timer_.deltaTime());
}

void Game::render()
{
    renderer_.clear();
    sceneManager_.render(renderer_);
    renderer_.present();
}

void Game::clean()
{
    if (SDL_WasInit(0) == 0U) {
        return;
    }

    running_ = false;
    sceneManager_.changeScene(nullptr);
    TextureManager::instance().clear();
    FontManager::instance().clear();
    AudioManager::instance().clear();
    renderer_.destroy();
    window_.destroy();
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    initialized_ = false;
}

bool Game::isRunning() const { return running_; }
Input& Game::input() { return input_; }
SceneManager& Game::scenes() { return sceneManager_; }

} // namespace SpaceInvaders
