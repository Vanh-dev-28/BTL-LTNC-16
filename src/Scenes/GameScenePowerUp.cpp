#include "Scenes/GameScene.h"

#include "Entities/PowerUp.h"

#include <algorithm>
#include <cstdlib>

namespace SpaceInvaders
{

void GameScene::updatePowerUps(float deltaTime)
{
    for (auto& powerUp : powerUps_)
    {
        powerUp.update(deltaTime);
    }

    powerUps_.erase(
        std::remove_if(
            powerUps_.begin(),
            powerUps_.end(),
            [](const PowerUp& powerUp)
            {
                return !powerUp.isActive();
            }
        ),
        powerUps_.end()
    );
}

void GameScene::checkPowerUpCollisions()
{
    constexpr float playerWidth = 48.0f;
    constexpr float playerHeight = 48.0f;

    for (auto& powerUp : powerUps_)
    {
        if (!powerUp.isActive())
        {
            continue;
        }

        if (powerUp.isCollectedBy(
                player_.x,
                player_.y,
                playerWidth,
                playerHeight))
        {
            activatePowerUp(powerUp.getType());
            powerUp.collect();
        }
    }
}

void GameScene::spawnPowerUp(float x, float y)
{
    constexpr int DROP_CHANCE = 25;

    if (rand() % 100 >= DROP_CHANCE)
    {
        return;
    }

    powerUps_.emplace_back(
        PowerUpType::ConeShot,
        x,
        y
    );
}

void GameScene::activatePowerUp(PowerUpType type)
{
    switch (type)
    {
    case PowerUpType::ConeShot:
        coneShotActive_ = true;
        coneShotTimer_ = CONE_SHOT_DURATION;
        break;
    }
}

}