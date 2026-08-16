#include "Scenes/GameScene.h"

#include "Core/Renderer.h"
#include "Utils/Constants.h"

#include <cstdlib>
#include <vector>
#include <algorithm>


namespace SpaceInvaders
{
    void GameScene::updateBullets(float deltaTime)
    {
        for (auto &bullet : bullets_)
        {
            bullet.update(deltaTime);
        }

        // Remove inactive bullets
        bullets_.erase(
            std::remove_if(bullets_.begin(), bullets_.end(), [](const Bullet &b)
                           { return !b.active; }),
            bullets_.end());
    }

    void GameScene::updateEnemies(float deltaTime)
    {
        // --- Enemy Shooting Logic ---
        enemyFireCooldown_ -= deltaTime;
        if (enemyFireCooldown_ <= 0.0f && !gameOver_)
        {
            std::vector<int> livingEnemyIndices;
            for (int i = 0; i < enemies_.size(); ++i)
            {
                if (enemies_[i].alive)
                {
                    livingEnemyIndices.push_back(i);
                }
            }

            if (!livingEnemyIndices.empty())
            {
                int shooterIndex = livingEnemyIndices[rand() % livingEnemyIndices.size()];
                const auto &shooter = enemies_[shooterIndex];
                // Spawn bullet from the center of the enemy
                bullets_.emplace_back(shooter.x, shooter.y + shooter.height, 250.0f, BulletOwner::Enemy);

                // Lấy tham chiếu đến viên đạn vừa tạo và tùy chỉnh kích thước
                Bullet &newBullet = bullets_.back();
                newBullet.width = 24.0f;  // Kích thước chiều rộng mới
                newBullet.height = 48.0f; // Kích thước chiều dài mới

                // Căn chỉnh lại vị trí để đạn bắn ra từ giữa tàu địch
                newBullet.x = shooter.x + (shooter.width / 2.0f) - (newBullet.width / 2.0f);
            }

            // Fire rate increases with waves
            float baseCooldown = 1.5f - (currentWave_ * 0.25f);                                 // Wave 1: 1.25, Wave 2: 1.0, Wave 3: 0.75
            enemyFireCooldown_ = baseCooldown + (static_cast<float>(rand()) / RAND_MAX) * 0.5f; // Add some randomness
        }

        bool hitEdge = false;
        for (auto &enemy : enemies_)
        {
            enemy.update(deltaTime, enemyDirection_);
            if (enemy.alive && (enemy.x < 20.0f || enemy.x > Constants::SCREEN_WIDTH - (enemy.width + 20.0f)))
            {
                hitEdge = true;
            }
        }

        if (hitEdge)
        {
            enemyDirection_ *= -1.0f;
            for (auto &enemy : enemies_)
            {
                if (enemy.alive)
                {
                    enemy.y += 18.0f;
                }
            }
        }

        for (const auto &enemy : enemies_)
        {
            if (enemy.alive && enemy.y + enemy.height >= player_.y)
            {
                gameOver_ = true;
                playerWon_ = false;
                break;
            }
        }
    }

    void GameScene::checkCollisions()
    {
        for (auto &bullet : bullets_)
        {
            if (!bullet.active)
            {
                continue;
            }

            if (bullet.owner == BulletOwner::Player)
            {
                // Check collision with enemies
                for (auto &enemy : enemies_)
                {
                    if (!enemy.alive)
                    {
                        continue;
                    }

                    // AABB collision check (rectangle-rectangle)
                    const bool hit = bullet.x < enemy.x + enemy.width &&
                                     bullet.x + bullet.width > enemy.x &&
                                     bullet.y < enemy.y + enemy.height &&
                                     bullet.y + bullet.height > enemy.y;
                    if (hit)
                    {
                        enemy.alive = false;
                        score_ += 10;

                        spawnPowerUp(enemy.x + enemy.width / 2.0f - 24.0f, enemy.y);
                        // Normal bullets are destroyed on impact, Fireball is not.
                        if (bullet.type != BulletType::Fireball)
                        {
                            bullet.active = false;
                            break; // A normal bullet only hits one enemy
                        }
                    }
                }
            }
            else // bullet.owner == BulletOwner::Enemy
            {
                // Check collision with player
                if (player_.isAlive())
                {
                    // Check collision with shield first
                    if (player_.isShieldActive())
                    {
                        const float shieldSize = 80.0f;
                        const float shieldX = player_.x + (48.0f - shieldSize) / 2.0f;
                        const float shieldY = player_.y + (48.0f - shieldSize) / 2.0f;
                        const bool shieldHit = bullet.x < shieldX + shieldSize &&
                                               bullet.x + bullet.width > shieldX &&
                                               bullet.y < shieldY + shieldSize &&
                                               bullet.y + bullet.height > shieldY;
                        if (shieldHit)
                        {
                            bullet.active = false;
                            continue; // Bullet destroyed, go to next bullet
                        }
                    }
                    const float playerWidth = 48.0f;
                    const float playerHeight = 48.0f;
                    // AABB collision check (rectangle-rectangle)
                    const bool hit = bullet.x < player_.x + playerWidth &&
                                     bullet.x + bullet.width > player_.x &&
                                     bullet.y < player_.y + playerHeight &&
                                     bullet.y + bullet.height > player_.y;
                    if (hit)
                    {
                        player_.takeDamage(Constants::ENEMY_LASER_DAMAGE);
                        bullet.active = false;
                    }
                }
            }
        }
    }
} // namespace SpaceInvaders