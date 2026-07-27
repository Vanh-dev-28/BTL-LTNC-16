#include "Core/Game.h"

#include <iostream>
#include "Scenes/Scene.h"
#include "Managers/AudioManager.h"
#include "Managers/FontManager.h"
#include "Managers/TextureManager.h"
#include "Utils/Constants.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Scenes/GameScene.h"
#include "Scenes/MenuScene.h"
#include <filesystem>

namespace SpaceInvaders
{

    Game::~Game()
    {
        clean();
    }

    bool Game::initialize()
    {
        if (initialized_)
        {
            return true;
        }

        // SỬA: SDL3 trả về true nếu thành công, dùng toán tử phủ định ! để kiểm tra thất bại
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
        {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            clean();
            return false;
        }

        // SỬA: TTF_Init trong SDL3 cũng trả về bool (true nếu thành công)
        if (!TTF_Init())
        {
            std::cerr << "TTF_Init failed: " << SDL_GetError() << std::endl;
            clean();
            return false;
        }

        if (!window_.create("Space Invaders", Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT))
        {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            clean();
            return false;
        }

        if (!renderer_.create(window_.getSDLWindow()))
        {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            clean();
            return false;
        }

        if (!AudioManager::instance().initialize())
        {
            std::cerr << "AudioManager initialization failed" << std::endl;
            clean();
            return false;
        }
        std::cout << "Current path: "
                  << std::filesystem::current_path()
                  << std::endl;

        // Font tiêu đề
        if (!FontManager::instance().loadFont(
                "menu_title",
                "../assets/fonts/Orbitron-Bold.ttf",
                72))
        {
            std::cerr << "Failed to load title font\n";
            clean();
            return false;
        }

        // Font menu
        if (!FontManager::instance().loadFont(
                "menu",
                "../assets/fonts/Orbitron-Regular.ttf",
                40))
        {
            std::cerr << "Failed to load menu font\n";
            clean();
            return false;
        }
        if (!TextureManager::instance().loadTexture(
                "menu_background",
                "../assets/image/menu/background.png",
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load menu background\n";
            clean();
            return false;
        }

        if (!TextureManager::instance().loadTexture(
                "gameplay_background",
                "../assets/image/gameplay_background.png",
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load gameplay background\n";
            clean();
            return false;
        }

        if (!TextureManager::instance().loadTexture(
                "bugs_invaders",
                "../assets/image/bugs_invaders.png",
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load bugs invaders sprite\n";
            clean();
            return false;
        }

        if (!TextureManager::instance().loadTexture(
                "ship",
                "../assets/image/ship.png",
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load ship sprite\n";
            clean();
            return false;
        }

        initialized_ = true;
        running_ = true;
        sceneManager_.changeScene(std::make_unique<MenuScene>());
        return true;
    }
    void Game::run()
    {
        while (running_)
        {
            handleEvents();
            update();
            render();
        }
    }

    void Game::handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_EVENT_QUIT)
            {
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
        if (SDL_WasInit(0) == 0U)
        {
            return;
        }

        running_ = false;
        sceneManager_.changeScene(nullptr);
        TextureManager::instance().clear();
        FontManager::instance().clear();
        AudioManager::instance().clear();
        renderer_.destroy();
        window_.destroy();
        TTF_Quit();
        SDL_Quit();
        initialized_ = false;
    }

    bool Game::isRunning() const { return running_; }
    Input &Game::input() { return input_; }
    SceneManager &Game::scenes() { return sceneManager_; }

} // namespace SpaceInvaders
