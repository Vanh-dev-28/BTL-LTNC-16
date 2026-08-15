#include "Scenes/GameScene.h"

#include "Utils/Constants.h"
#include "Utils/Vector2.h"

namespace SpaceInvaders
{

void GameScene::resetWave()
{
    enemies_.clear();
    bullets_.clear();
    powerUps_.clear();

    enemyDirection_ = 1.0f;

    switch (currentWave_)
    {
    case 1:
    {
        const int columns = 8;
        const int rows = 3;
        const float speed = 60.0f;

        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < columns; ++col)
            {
                Vector2 targetPos = {
                    150.0f + col * 80.0f,
                    100.0f + row * 60.0f
                };

                Vector2 startPos = {
                    targetPos.x,
                    -50.0f - row * 60.0f
                };

                enemies_.emplace_back(
                    EnemyType::Drone,
                    EnemyMovementPattern::Horizontal,
                    EnemyEntryPattern::FromTop,
                    startPos,
                    targetPos,
                    speed
                );
            }
        }

        break;
    }

    case 2:
    {
        const int perSide = 8;
        const float speed = 75.0f;

        for (int i = 0; i < perSide; ++i)
        {
            // LEFT

            Vector2 targetPosL = {
                200.0f + (i % 4) * 100.0f,
                120.0f + (i / 4) * 70.0f
            };

            Vector2 startPosL = {
                -100.0f,
                100.0f + i * 20.0f
            };

            enemies_.emplace_back(
                EnemyType::Bomber,
                EnemyMovementPattern::SineWave,
                EnemyEntryPattern::ArcFromLeft,
                startPosL,
                targetPosL,
                speed
            );

            // RIGHT

            Vector2 targetPosR = {
                Constants::SCREEN_WIDTH -
                    200.0f -
                    (i % 4) * 100.0f,

                120.0f + (i / 4) * 70.0f
            };

            Vector2 startPosR = {
                Constants::SCREEN_WIDTH + 100.0f,
                100.0f + i * 20.0f
            };

            enemies_.emplace_back(
                EnemyType::Bomber,
                EnemyMovementPattern::SineWave,
                EnemyEntryPattern::ArcFromRight,
                startPosR,
                targetPosR,
                speed
            );
        }

        break;
    }

    case 3:
    {
        const float speed = 90.0f;

        // From top

        for (int i = 0; i < 5; ++i)
        {
            Vector2 targetPos = {
                340.0f + i * 120.0f,
                250.0f
            };

            Vector2 startPos = {
                targetPos.x,
                -50.0f
            };

            enemies_.emplace_back(
                EnemyType::HealthSpaceship,
                EnemyMovementPattern::Horizontal,
                EnemyEntryPattern::FromTop,
                startPos,
                targetPos,
                speed
            );
        }

        // From sides

        for (int i = 0; i < 6; ++i)
        {
            // LEFT

            Vector2 targetPosL = {
                150.0f + (i % 3) * 100.0f,
                100.0f + (i / 3) * 60.0f
            };

            Vector2 startPosL = {
                -100.0f,
                150.0f + i * 15.0f
            };

            enemies_.emplace_back(
                EnemyType::Drone,
                EnemyMovementPattern::SineWave,
                EnemyEntryPattern::ArcFromLeft,
                startPosL,
                targetPosL,
                speed
            );

            // RIGHT

            Vector2 targetPosR = {
                Constants::SCREEN_WIDTH -
                    150.0f -
                    (i % 3) * 100.0f,

                100.0f + (i / 3) * 60.0f
            };

            Vector2 startPosR = {
                Constants::SCREEN_WIDTH + 100.0f,
                150.0f + i * 15.0f
            };

            enemies_.emplace_back(
                EnemyType::Drone,
                EnemyMovementPattern::SineWave,
                EnemyEntryPattern::ArcFromRight,
                startPosR,
                targetPosR,
                speed
            );
        }

        break;
    }
    }
}

bool GameScene::allEnemiesDefeated() const
{
    if (inWaveTransition_ || enemies_.empty())
    {
        return false;
    }

    for (const auto& enemy : enemies_)
    {
        if (enemy.alive)
        {
            return false;
        }
    }

    return true;
}

}