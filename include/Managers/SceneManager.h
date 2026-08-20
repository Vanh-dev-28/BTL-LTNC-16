#pragma once

#include "Scenes/Scene.h"
#include <memory>

namespace SpaceInvaders 
{
    class Renderer;
    class Input;
    class SceneManager {
    public:
        ~SceneManager();
        void changeScene(std::unique_ptr<Scene> scene);
        [[nodiscard]] Scene* currentScene() const;
        void update(float deltaTime);
        void render(Renderer& renderer);
        void setInput(Input* input);
        [[nodiscard]]
        Input& input() const;
    private:
        std::unique_ptr<Scene> currentScene_;
        Input* input_ = nullptr;
    };
} // namespace SpaceInvaders
