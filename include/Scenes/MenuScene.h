#pragma once

#include "Scenes/Scene.h"
#include <vector>
#include <string>

namespace SpaceInvaders
{

class MenuScene : public Scene
{
public:
    MenuScene() = default;
    ~MenuScene() override = default;

    void enter() override;
    void exit() override;

    void update(float deltaTime) override;
    void render(Renderer& renderer) override;

private:
    std::vector<std::string> menuItems_
    {
        "Start Game",
        "Ranking",
        "Settings",
        "Exit"
    };

    int selectedIndex_ = 0;
};

}