#pragma once

#include "Core/Input.h"
#include "Core/Renderer.h"
#include "Core/Timer.h"
#include "Core/Window.h"
#include "Managers/SceneManager.h"

namespace SpaceInvaders {

/** Coordinates SDL subsystems and the main application loop. */
class Game {
public:
    Game() = default;
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    bool initialize();
    void run();
    void handleEvents();
    void update();
    void render();
    void clean();

    [[nodiscard]] bool isRunning() const;
    [[nodiscard]] Input& input();
    [[nodiscard]] SceneManager& scenes();

private:
    bool running_ {false};
    bool initialized_ {false};
    Window window_;
    Renderer renderer_;
    Input input_;
    Timer timer_;
    SceneManager sceneManager_;
};

} // namespace SpaceInvaders
