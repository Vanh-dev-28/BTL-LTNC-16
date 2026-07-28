#include "Scenes/Scene.h"
#include "Managers/SceneManager.h"
#include "Core/Input.h"

namespace SpaceInvaders
{

Input& Scene::input() const
{
    return sceneManager_->input();
}

} // namespace SpaceInvaders