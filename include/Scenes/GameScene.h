#pragma once

#include "Scenes/Scene.h"

namespace SpaceInvaders
{

class GameScene : public Scene
{
public:
    GameScene() = default;
    ~GameScene() override = default;

    void enter() override;
    void exit() override;

    void update(float deltaTime) override;
    void render(Renderer& renderer) override;
};

}