#include "Core/Game.h"

#include <iostream>
#include "Scenes/Scene.h"
#include "Managers/AudioManager.h"
#include "Managers/FontManager.h"
#include "Managers/TextureManager.h"
#include "Managers/SettingsManager.h"
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
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
        {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            clean();
            return false;
        }
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

        const std::filesystem::path assetRoot =
            (std::filesystem::current_path() / ".." / "assets").lexically_normal();

        std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
        std::cout << "Resolved asset root: " << assetRoot << std::endl;

        AudioManager &audio = AudioManager::instance();
        audio.setMusicVolume(SettingsManager::instance().getMusicVolume());

        if (!audio.playMusic((assetRoot / "audio" / "music" / "background_music.mp3").string()))
        {
            std::cerr << "Failed to play background music\n";
        }

        if (!FontManager::instance().loadFont(
                "menu_title",
                (assetRoot / "fonts" / "Orbitron-Bold.ttf").string(),
                72))
        {
            std::cerr << "Failed to load title font from '"
                      << (assetRoot / "fonts" / "Orbitron-Bold.ttf")
                      << "'\n";
            clean();
            return false;
        }

        if (!FontManager::instance().loadFont(
                "menu",
                (assetRoot / "fonts" / "Orbitron-Regular.ttf").string(),
                40))
        {
            std::cerr << "Failed to load menu font from '"
                      << (assetRoot / "fonts" / "Orbitron-Regular.ttf")
                      << "'\n";
            clean();
            return false;
        }
        if (!TextureManager::instance().loadTexture(
                "menu_background",
                (assetRoot / "image" / "menu" / "background.png").string(),
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load menu background from '"
                      << (assetRoot / "image" / "menu" / "background.png")
                      << "'\n";
            clean();
            return false;
        }
        if (!TextureManager::instance().loadTexture(
                "gameplay_background",
                (assetRoot / "image" / "Background" / "gameplay_background.png").string(),
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load gameplay background from '"
                      << (assetRoot / "image" / "Background" / "gameplay_background.png")
                      << "': " << SDL_GetError() << "\n";
            clean();
            return false;
        }
        if (!TextureManager::instance().loadTexture(
                "settings_background",
                "../assets/image/menu/settings_bg.png",
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load settings background\n";
            clean();
            return false;
        }

        if (!TextureManager::instance().loadTexture(
                "resolution_popup",
                "../assets/image/menu/popup_bg.png",
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load resolution popup\n";
            clean();
            return false;
        }

        if (!TextureManager::instance().loadTexture(
        "endgame_popup",
        "../assets/image/menu/endgame_popup.png",
        renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load endgame popup\n";
            clean();
            return false;
        }

        if (!TextureManager::instance().loadTexture(
                "enemy_bomber",
                "../assets/image/Enemies/bomber.png",
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load enemy_bomber sprite\n";
            clean();
            return false;
        }
        if (!TextureManager::instance().loadTexture(
                "enemy_drone",
                "../assets/image/Enemies/drone.png",
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load enemy_drone sprite\n";
            clean();
            return false;
        }
        if (!TextureManager::instance().loadTexture(
                "enemy_health_spaceship",
                "../assets/image/Enemies/health-spaceship.png",
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load enemy_health_spaceship sprite\n";
            clean();
            return false;
        }

        if (!TextureManager::instance().loadTexture(
                "ship",
                "../assets/image/Ships/fighter.png",
                renderer_.getSDLRenderer()))
        {
            std::cerr << "Failed to load ship sprite\n";
            clean();
            return false;
        }

        initialized_ = true;
        running_ = true;
        sceneManager_.setInput(&input_);
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
        applySettings();
    }

    void Game::applySettings()
    {
        SettingsManager &settings = SettingsManager::instance();

        if (!settings.consumeApplyRequest())
        {
            return;
        }

        const std::string &resolution = settings.getResolution();

        int width = 1280;
        int height = 720;

        if (resolution == "1600x900")
        {
            width = 1600;
            height = 900;
        }
        else if (resolution == "1920x1080")
        {
            width = 1920;
            height = 1080;
        }

        window_.setSize(width, height);
        // Kết nối với AudioManager để thay đổi âm lượng nhạc nền
        AudioManager::instance().setMusicVolume(settings.getMusicVolume());
    }

    void Game::render()
    {
        renderer_.updateViewport(window_.getSDLWindow());

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