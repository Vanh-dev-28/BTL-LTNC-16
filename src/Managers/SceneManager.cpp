#include "Managers/SceneManager.h"

#include "Scenes/Scene.h"

namespace SpaceInvaders {

SceneManager::~SceneManager() = default;

void SceneManager::changeScene(std::unique_ptr<Scene> scene)
{
    if (currentScene_ != nullptr) {
        currentScene_->exit();
    }
    currentScene_ = std::move(scene);
    if (currentScene_ != nullptr) {
        currentScene_->enter();
    }
}

Scene* SceneManager::currentScene() const { return currentScene_.get(); }

void SceneManager::update(float deltaTime)
{
    if (currentScene_ != nullptr) {
        currentScene_->update(deltaTime);
    }
}

void SceneManager::render(Renderer& renderer)
{
    if (currentScene_ != nullptr) {
        currentScene_->render(renderer);
    }
}

} // namespace SpaceInvaders
