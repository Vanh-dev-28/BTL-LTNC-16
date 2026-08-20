#include "Scenes/GameScene.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <algorithm>
#include <cmath> // For M_PI and cos
#include "Utils/Constants.h"
#include "Entities/Enemy.h"
#include "Utils/Vector2.h"

namespace SpaceInvaders
{

    void GameScene::resetWave()
    {
        enemies_.clear();
        pendingEnemies_.clear();
        bullets_.clear();
        powerUps_.clear();

        spawnTimer_ = 0.0f;
        enemyDirection_ = 1.0f;

        switch (currentWave_)
        {
        case 1:
        {
            // Wave 1: 24 Drones in 3 sequential groups of 8
            const int cols = 8;
            const float spacingX = 60.0f; // Cân chỉnh lại 60px để đội hình gọn gàng, không bị chạm biên sớm
            const float startX = (Constants::SCREEN_WIDTH - (cols - 1) * spacingX) / 2.0f;

            // Helper to spawn a row
            auto spawn_row = [&](float targetY, float speed, bool last_group)
            {
                for (int col = 0; col < cols; ++col)
                {
                    Vector2 targetPos = {startX + col * spacingX, targetY};
                    Vector2 startPos = {targetPos.x, -50.0f};
                    float delay = (col == cols - 1 && !last_group) ? 2.0f : 0.15f;

                    pendingEnemies_.push_back({EnemyType::Drone,
                                               EnemyMovementPattern::Horizontal,
                                               EnemyEntryPattern::FromTop,
                                               startPos,
                                               targetPos,
                                               speed,
                                               {0, 0},
                                               {0, 0}, // No bezier control points
                                               delay});
                }
            };

            // Group 1 (Row 1)
            spawn_row(100.0f, 60.0f, false);

            // Group 2 (Row 2)
            spawn_row(160.0f, 60.0f, false);

            // Group 3 (Row 3)
            spawn_row(220.0f, 70.0f, true);
            break;
        }

        case 2:
        {
            // 20 total: 12 Drone, 8 Bomber
            std::vector<EnemyType> types;
            types.insert(types.end(), 12, EnemyType::Drone);
            types.insert(types.end(), 8, EnemyType::Bomber);
            std::random_shuffle(types.begin(), types.end());

            const int rows = 4;
            const int cols = 5;
            const float spacingX = 75.0f;
            const float spacingY = 60.0f;
            const float startX = (Constants::SCREEN_WIDTH - (cols - 1) * spacingX) / 2.0f;
            const float startY = 100.0f;
            const float speed = 60.0f + (currentWave_ * 10.0f);
            int type_idx = 0;

            for (int row = 0; row < rows; ++row)
            {
                for (int col = 0; col < cols; ++col)
                {
                    Vector2 targetPos = {startX + col * spacingX, startY + row * spacingY};
                    if (col < (cols + 1) / 2) // Left side
                    {
                        Vector2 startPos = {-100.0f, 200.0f + row * 40.0f};
                        Vector2 c1 = {startPos.x + 400.0f, startPos.y - 150.0f};
                        Vector2 c2 = {targetPos.x - 100.0f, targetPos.y + 250.0f};
                        pendingEnemies_.push_back({types[type_idx++], EnemyMovementPattern::SineWave, EnemyEntryPattern::Galaga, startPos, targetPos, speed, c1, c2});
                    }
                    else // Right side
                    {
                        Vector2 startPos = {Constants::SCREEN_WIDTH + 100.0f, 200.0f + row * 40.0f};
                        Vector2 c1 = {startPos.x - 400.0f, startPos.y - 150.0f};
                        Vector2 c2 = {targetPos.x + 100.0f, targetPos.y + 250.0f};
                        pendingEnemies_.push_back({types[type_idx++], EnemyMovementPattern::SineWave, EnemyEntryPattern::Galaga, startPos, targetPos, speed, c1, c2});
                    }
                }
            }
            break;
        }

        case 3:
        {
            // 24 total: 12 Drone, 7 Bomber, 5 HealthSpaceship
            std::vector<EnemyType> types;
            types.insert(types.end(), 12, EnemyType::Drone);
            types.insert(types.end(), 7, EnemyType::Bomber);
            types.insert(types.end(), 5, EnemyType::HealthSpaceship);
            std::random_shuffle(types.begin(), types.end());

            // Wave 3: Top group + two curved side groups
            const int rows = 4;
            const int cols = 6;
            const float spacingX = 70.0f;
            const float spacingY = 55.0f;
            const float startX = (Constants::SCREEN_WIDTH - (cols - 1) * spacingX) / 2.0f;
            const float startY = 100.0f;
            const float speed = 60.0f + (currentWave_ * 10.0f);

            for (int i = 0; i < types.size(); ++i)
            {
                int row = i / cols;
                int col = i % cols;
                Vector2 targetPos = {startX + col * spacingX, startY + row * spacingY};

                if (row < 2)
                { // Top group
                    Vector2 startPos = {targetPos.x, -50.0f};
                    pendingEnemies_.push_back({types[i], EnemyMovementPattern::SineWave, EnemyEntryPattern::FromTop, startPos, targetPos, speed});
                }
                else
                { // Side groups
                    Vector2 startPos = (col < cols / 2) ? Vector2{-100.0f, 300.0f} : Vector2{Constants::SCREEN_WIDTH + 100.0f, 300.0f};
                    Vector2 c1 = {Constants::SCREEN_WIDTH / 2.0f, 100.0f};
                    Vector2 c2 = {targetPos.x, Constants::SCREEN_HEIGHT};
                    pendingEnemies_.push_back({types[i], EnemyMovementPattern::SineWave, EnemyEntryPattern::Galaga, startPos, targetPos, speed, c1, c2});
                }
            }
            break;
        }

        case 4:
        {
            // 28 total: 11 Drone, 11 Bomber, 6 HealthSpaceship
            // Entry: Four corners with crossing curved paths
            std::vector<EnemyType> types;
            types.insert(types.end(), 11, EnemyType::Drone);
            types.insert(types.end(), 11, EnemyType::Bomber);
            types.insert(types.end(), 6, EnemyType::HealthSpaceship);
            std::random_shuffle(types.begin(), types.end());

            const int rows = 4;
            const int cols = 7;
            const float spacingX = 65.0f;
            const float spacingY = 55.0f;
            const float startX = (Constants::SCREEN_WIDTH - (cols - 1) * spacingX) / 2.0f;
            const float startY = 100.0f;
            const float speed = 60.0f + (currentWave_ * 10.0f);

            for (int i = 0; i < rows * cols; ++i)
            {
                int row = i / cols;
                int col = i % cols;
                Vector2 targetPos = {startX + col * spacingX, startY + row * spacingY};
                Vector2 startPos, c1, c2;

                switch (i % 4)
                {
                case 0: // Top-left
                    startPos = {-100.0f, -100.0f};
                    c1 = {Constants::SCREEN_WIDTH * 0.8f, 50.0f};
                    c2 = {Constants::SCREEN_WIDTH * 0.2f, Constants::SCREEN_HEIGHT * 0.7f};
                    break;
                case 1: // Top-right
                    startPos = {Constants::SCREEN_WIDTH + 100.0f, -100.0f};
                    c1 = {Constants::SCREEN_WIDTH * 0.2f, 50.0f};
                    c2 = {Constants::SCREEN_WIDTH * 0.8f, Constants::SCREEN_HEIGHT * 0.7f};
                    break;
                case 2: // Bottom-left
                    startPos = {-100.0f, Constants::SCREEN_HEIGHT + 100.0f};
                    c1 = {Constants::SCREEN_WIDTH * 0.3f, Constants::SCREEN_HEIGHT * 0.8f};
                    c2 = {Constants::SCREEN_WIDTH * 0.7f, 50.0f};
                    break;
                case 3: // Bottom-right
                    startPos = {Constants::SCREEN_WIDTH + 100.0f, Constants::SCREEN_HEIGHT + 100.0f};
                    c1 = {Constants::SCREEN_WIDTH * 0.7f, Constants::SCREEN_HEIGHT * 0.8f};
                    c2 = {Constants::SCREEN_WIDTH * 0.3f, 50.0f};
                    break;
                }
                pendingEnemies_.push_back({types[i], EnemyMovementPattern::Expansion, EnemyEntryPattern::Galaga, startPos, targetPos, speed, c1, c2});
            }
            break;
        }

        case 5:
        {
            // 32 total: 6 Drone, 13 Bomber, 13 HealthSpaceship
            // Entry: Four continuous spiral groups
            std::vector<EnemyType> types;
            types.insert(types.end(), 6, EnemyType::Drone);
            types.insert(types.end(), 13, EnemyType::Bomber);
            types.insert(types.end(), 13, EnemyType::HealthSpaceship);
            std::random_shuffle(types.begin(), types.end());

            const int rows = 4;
            const int cols = 8;
            const float spacingX = 60.0f;
            const float spacingY = 50.0f;
            const float startX = (Constants::SCREEN_WIDTH - (cols - 1) * spacingX) / 2.0f;
            const float startY = 110.0f;
            const float speed = 70.0f + (currentWave_ * 10.0f);

            for (int i = 0; i < rows * cols; ++i)
            {
                int row = i / cols;
                int col = i % cols;
                Vector2 targetPos = {startX + col * spacingX, startY + row * spacingY};
                Vector2 startPos, c1, c2;

                // Four spiral groups
                float angle = (i % 4) * (M_PI / 2.0f) + (i / 4) * 0.3f;
                startPos = {Constants::SCREEN_WIDTH / 2.0f + static_cast<float>(cos(angle)) * 800.0f, -200.0f};
                c1 = {Constants::SCREEN_WIDTH / 2.0f, Constants::SCREEN_HEIGHT * 1.2f};
                c2 = {targetPos.x + (col < cols / 2 ? -300.0f : 300.0f), targetPos.y - 300.0f};

                pendingEnemies_.push_back({types[i], EnemyMovementPattern::ZigZag, EnemyEntryPattern::Galaga, startPos, targetPos, speed * 1.2f, c1, c2});
            }
            break;
        }
        }
    }

    bool GameScene::allEnemiesDefeated() const
    {
        if (inWaveTransition_ || !pendingEnemies_.empty())
        {
            return false;
        }

        for (const auto &enemy : enemies_)
        {
            if (enemy.alive)
            {
                return false;
            }
        }

        return true;
    }

}