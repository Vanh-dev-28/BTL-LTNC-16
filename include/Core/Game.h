#pragma once

#include "Core/Input.h"
#include "Core/Renderer.h"
#include "Core/Timer.h"
#include "Core/Window.h"
#include "Managers/SceneManager.h"

namespace SpaceInvaders
{

    class Game
    {
    public:
        Game() = default;
        ~Game();

        Game(const Game &) = delete;
        Game &operator=(const Game &) = delete;

        bool initialize();

        void run();

        void handleEvents();

        void update();

        void render();

        void clean();

        void applySettings();

        [[nodiscard]] bool isRunning() const;

        [[nodiscard]] Input &input();

        [[nodiscard]] SceneManager &scenes();

    private:
        bool running_{false};

        bool initialized_{false};

        Window window_;

        Renderer renderer_;

        Input input_;

        Timer timer_;

        /** Quản lý các màn chơi / cảnh trong game (Menu, Play, End Game). */
        SceneManager sceneManager_;
    };

} // namespace SpaceInvaders