#pragma once

#include "Scenes/Scene.h"
#include <memory>

namespace SpaceInvaders {

class Renderer;

/** Owns the active scene and delegates its frame lifecycle. */
class SceneManager {
public:
    ~SceneManager();

    void changeScene(std::unique_ptr<Scene> scene);
    [[nodiscard]] Scene* currentScene() const;
    void update(float deltaTime);
    void render(Renderer& renderer);

private:
    std::unique_ptr<Scene> currentScene_;
};

} // namespace SpaceInvaders
