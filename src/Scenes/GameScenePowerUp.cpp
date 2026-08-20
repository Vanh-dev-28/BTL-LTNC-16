#include "Scenes/GameScene.h"
#include "Core/Renderer.h"
#include "Core/Input.h"
#include "Managers/TextureManager.h"
#include "Managers/AudioManager.h"
#include "Entities/PowerUp.h"
#include "Entities/CompanionShip.h"
#include "Utils/Constants.h"
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace SpaceInvaders
{
    void GameScene::updatePowerUps(float deltaTime)
    {
        for (auto& powerUp : powerUps_)
        {
            powerUp.update(deltaTime);
        }
        powerUps_.erase(std::remove_if(powerUps_.begin(), powerUps_.end(), [](const PowerUp& powerUp)
            {   return !powerUp.isActive();    }),
        powerUps_.end());
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

            if (powerUp.isCollectedBy(player_.x, player_.y, playerWidth, playerHeight))
            {
                if (powerUp.getType() == PowerUpType::Heal)
                {
                    if (player_.getHealth() >= player_.getMaxHealth())
                    {
                        continue;
                    }
                }
                activatePowerUp(powerUp.getType());
                powerUp.collect();
            }
        }
    }

    void GameScene::spawnPowerUp(float x, float y)
    {
        const int roll = rand() % 140;
        if (roll < 10)
        {
            powerUps_.emplace_back(PowerUpType::ConeShot, x, y);
            return;
        }
        if (roll < 20)
        {
            powerUps_.emplace_back(PowerUpType::Heal, x, y);
            return;
        }
        if (roll < 30)
        {
            powerUps_.emplace_back(PowerUpType::Companion, x, y);
            return;
        }
    }

    void GameScene::activatePowerUp(PowerUpType type)
    {
    const std::filesystem::path assetRoot = (std::filesystem::current_path() / ".." / "assets").lexically_normal();

    switch (type)
    {
    case PowerUpType::ConeShot:
        coneShotActive_ = true;
        coneShotTimer_ = CONE_SHOT_DURATION;

        AudioManager::instance().playSFX(
            (assetRoot / "audio" / "powerup_sf" / "cone-shot.mp3").string()
        );
        break;

    case PowerUpType::Heal:
        player_.heal(20.0f);

        AudioManager::instance().playSFX(
            (assetRoot / "audio" / "powerup_sf" / "heal.mp3").string()
        );
        break;

    case PowerUpType::Companion:
        spawnCompanions();

        AudioManager::instance().playSFX(
            (assetRoot / "audio" / "powerup_sf" / "companion.mp3").string()
        );
        break;
    }
}
    void GameScene::spawnCompanions()
    {
        companions_.clear();

        constexpr float playerWidth = 48.0f;
        constexpr float playerHeight = 48.0f;

        const float playerCenterX = player_.x + playerWidth / 2.0f;
        const float playerCenterY = player_.y + playerHeight / 2.0f;
        constexpr float companionDistance = 140.0f;

        const float companionXOffset = companionDistance + 24.0f;
        const float companionY = playerCenterY - 24.0f;
    companions_.emplace_back( CompanionSide::Left, playerCenterX - companionXOffset, companionY );
    companions_.emplace_back( CompanionSide::Right, playerCenterX + companionDistance, companionY );
    }

    void GameScene::updateCompanion(float deltaTime)
    {
        if (companions_.empty())
        {
            return;
        }
        moveCompanion(deltaTime);
        for (auto& companion : companions_)
        {
            if (!companion.isActive())
            {
                continue;
            }
            companion.shoot(bullets_);
        }
        companions_.erase(std::remove_if(companions_.begin(), companions_.end(), [](const CompanionShip& companion)
            { return !companion.isActive(); } ),companions_.end() );
    }

    void GameScene::moveCompanion(float deltaTime)
    {
        if (companions_.empty())
        {
            return;
        }

        constexpr float playerWidth = 48.0f;
        constexpr float playerHeight = 48.0f;
        const float playerX = player_.x + playerWidth / 2.0f;
        const float playerY = player_.y + playerHeight / 2.0f;
        for (auto& companion : companions_)
        {
            if (!companion.isActive())
            {
                continue;
            }
        
            companion.update(deltaTime, playerX, playerY);
        }
    }

    void GameScene::checkCompanionCollision()
    {
        if (companions_.empty())
        {
            return;
        }

        for (auto& bullet : bullets_)
        {
            if (!bullet.active)
            {
                continue;
            }

            if (bullet.owner != BulletOwner::Enemy)
            {
                continue;
            }

            const SDL_FRect bulletHitbox = bullet.getHitbox();
            for (auto& companion : companions_)
            {
                if (!companion.isActive())
                {
                    continue;
                }   

                const SDL_FRect companionHitbox = companion.getHitbox();
                if (SDL_HasRectIntersectionFloat(&bulletHitbox, &companionHitbox))
                {
                    companion.destroy();
                    bullet.active = false;
                    break;
                }
            }
        }
    }

    void GameScene::renderCompanion(Renderer& renderer)
    {
        for (const auto& companion : companions_)
        {
            if (!companion.isActive())
                continue;
            companion.render(renderer);
        }
    }
}