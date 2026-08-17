#include "Scenes/GameScene.h"

#include <cmath>
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
        // --- Handle Wave Spawning ---
        if (!pendingEnemies_.empty())
        {
            spawnTimer_ -= deltaTime;
            if (spawnTimer_ <= 0.0f)
            {
                const auto &spawnData = pendingEnemies_.front();

                // Create an enemy from the spawn data
                enemies_.emplace_back(
                    spawnData.type,
                    spawnData.movePattern,
                    spawnData.entryPattern,
                    spawnData.startPos,
                    spawnData.targetPos,
                    spawnData.speed,
                    spawnData.c1,
                    spawnData.c2);

                // Set timer for the next spawn using the just-spawned enemy's delay
                spawnTimer_ = spawnData.spawnDelay;

                pendingEnemies_.erase(pendingEnemies_.begin());
            }
        }

        m_swarmSharedTime += deltaTime;

        // --- Task E: Expansion/Contraction ---
        const float SCALE_SPEED = 0.2f;
        const float MIN_SCALE = 0.8f;
        const float MAX_SCALE = 1.2f;
        m_formationScale += m_formationScaleDirection * SCALE_SPEED * deltaTime;
        if (m_formationScale > MAX_SCALE || m_formationScale < MIN_SCALE)
        {
            m_formationScaleDirection *= -1.0f;
            m_formationScale = std::clamp(m_formationScale, MIN_SCALE, MAX_SCALE);
        }

        // --- Task C: Dive-bomb trigger ---
        m_diveAttackTimer -= deltaTime;
        if (m_diveAttackTimer <= 0.0f)
        {
            if (currentWave_ >= 3) // Dive attacks start from Wave 3
            {
                std::vector<int> available_enemies;
                for (int i = 0; i < enemies_.size(); ++i)
                {
                    if (enemies_[i].alive && enemies_[i].getState() == EnemyState::Active)
                    {
                        available_enemies.push_back(i);
                    }
                }

                if (!available_enemies.empty())
                {
                    int divers = 1;
                    if (currentWave_ == 4 || currentWave_ == 5)
                    {
                        divers = 2;
                    }

                    for (int i = 0; i < divers && !available_enemies.empty(); ++i)
                    {
                        int rand_idx = rand() % available_enemies.size();
                        int enemy_idx = available_enemies[rand_idx];

                        available_enemies.erase(available_enemies.begin() + rand_idx);

                        enemies_[enemy_idx].startDive({player_.x, player_.y},
                                                      (rand() % 2 == 0) ? EnemyDivePattern::Straight : EnemyDivePattern::Curved);
                    }
                }
            }

            // Đặt lại thời gian hồi chiêu hợp lý để người chơi có khoảng trống phản xạ
            if (currentWave_ == 5)
            {
                m_diveAttackTimer = 2.0f + (static_cast<float>(rand()) / RAND_MAX) * 1.5f; // 2.0s - 3.5s
            }
            else
            {
                m_diveAttackTimer = 3.5f + (static_cast<float>(rand()) / RAND_MAX) * 3.0f; // 3.5s - 6.5s
            }
        }

        bool hitEdge = false;
        float minX = Constants::SCREEN_WIDTH, maxX = 0;

        for (auto &enemy : enemies_)
        {
            if (enemy.alive)
            {
                Vector2 swarmVelocity = {0, 0};
                if (enemy.getState() == EnemyState::Active)
                {
                    // Base horizontal movement
                    swarmVelocity.x = enemyDirection_ * enemy.getSpeed() * 0.5f;

                    // --- Apply swarm movement patterns ---
                    if (enemy.getMovementPattern() == EnemyMovementPattern::ZigZag)
                    {
                        swarmVelocity.x += 150.0f * cos(m_swarmSharedTime * 2.0f);
                    }
                    else if (enemy.getMovementPattern() == EnemyMovementPattern::Vortex)
                    {
                        const Vector2 &targetPos = enemy.getTargetPosition();
                        float dx = targetPos.x - m_formationCenter.x;
                        float dy = targetPos.y - m_formationCenter.y;
                        float radius = sqrt(dx * dx + dy * dy);
                        float baseAngle = atan2(dy, dx);
                        float currentAngle = baseAngle + m_swarmSharedTime * 1.5f;

                        float targetX = m_formationCenter.x + cos(currentAngle) * radius;
                        float targetY = m_formationCenter.y + sin(currentAngle) * radius;

                        swarmVelocity.x = (targetX - enemy.x) * 2.0f;
                        swarmVelocity.y = (targetY - enemy.y) * 2.0f;
                    }
                    else if (enemy.getMovementPattern() == EnemyMovementPattern::Expansion)
                    {
                        const Vector2 &targetPos = enemy.getTargetPosition();
                        float targetX = m_formationCenter.x + (targetPos.x - m_formationCenter.x) * m_formationScale;
                        float targetY = m_formationCenter.y + (targetPos.y - m_formationCenter.y) * m_formationScale;

                        swarmVelocity.x += (targetX - enemy.x) * 1.0f;
                        swarmVelocity.y += (targetY - enemy.y) * 1.0f;
                    }
                }

                enemy.update(deltaTime, swarmVelocity);

                if (enemy.getState() == EnemyState::Active)
                {
                    if (enemy.x < minX)
                        minX = enemy.x;
                    if (enemy.x > maxX)
                        maxX = enemy.x;
                }

                // Task C: Shooting while diving
                if (enemy.getState() == EnemyState::Diving && (rand() % 150 == 0))
                {
                    const float bulletWidth = 8.0f;
                    float bulletX = enemy.x + (enemy.width - bulletWidth) / 2.0f;
                    bullets_.emplace_back(bulletX, enemy.y + enemy.height, 350.0f, BulletOwner::Enemy);
                }
            }
        }

        if (minX < 20.0f || maxX > Constants::SCREEN_WIDTH - 68.0f)
        {
            hitEdge = true;
        }

        if (hitEdge)
        {
            enemyDirection_ *= -1.0f;
            for (auto &enemy : enemies_)
            {
                if (enemy.alive && enemy.getState() == EnemyState::Active)
                {
                    // Giảm khoảng cách tụt xuống còn 6px để tránh rơi nhanh
                    enemy.y += 6.0f;
                }
            }
        }

        // Kiểm tra Game Over với khoảng đệm an toàn
        for (const auto &enemy : enemies_)
        {
            // CHỈ xử thua khi quái lọt hẳn xuống mép đáy màn hình (ví dụ SCREEN_HEIGHT - 30px)
            if (enemy.alive &&
                enemy.getState() == EnemyState::Active &&
                (enemy.y + enemy.height >= Constants::SCREEN_HEIGHT - 30.0f))
            {
                gameOver_ = true;
                playerWon_ = false;
                break;
            }
        }

        // --- Enemy Swarm Shooting Logic ---
        enemyFireCooldown_ -= deltaTime;
        if (enemyFireCooldown_ <= 0.0f && !gameOver_)
        {
            std::vector<int> livingEnemyIndices;
            for (int i = 0; i < enemies_.size(); ++i)
            {
                if (enemies_[i].alive && enemies_[i].getState() == EnemyState::Active)
                {
                    livingEnemyIndices.push_back(i);
                }
            }

            if (!livingEnemyIndices.empty())
            {
                int shooterIndex = livingEnemyIndices[rand() % livingEnemyIndices.size()];
                const auto &shooter = enemies_[shooterIndex];
                const float bulletWidth = 8.0f;
                float bulletX = shooter.x + (shooter.width - bulletWidth) / 2.0f;
                bullets_.emplace_back(bulletX, shooter.y + shooter.height, 250.0f, BulletOwner::Enemy);
            }

            float baseCooldown = 1.5f - (currentWave_ * 0.15f);
            enemyFireCooldown_ = std::max(0.25f, baseCooldown) + (static_cast<float>(rand()) / RAND_MAX) * 0.5f;
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

                        if (bullet.type != BulletType::Fireball)
                        {
                            bullet.active = false;
                            break;
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
                            continue;
                        }
                    }

                    // Make the player's hitbox smaller than the sprite for more forgiving gameplay.
                    // This ignores the "wings" and makes it feel fairer.
                    const float playerSpriteWidth = 48.0f;
                    const float playerSpriteHeight = 48.0f;
                    const float playerHitboxWidth = 28.0f;  // Smaller hitbox width
                    const float playerHitboxHeight = 28.0f; // Smaller hitbox height
                    const float hitboxOffsetX = (playerSpriteWidth - playerHitboxWidth) / 2.0f;
                    const float hitboxOffsetY = (playerSpriteHeight - playerHitboxHeight) / 2.0f;

                    const float playerHitboxX = player_.x + hitboxOffsetX;
                    const float playerHitboxY = player_.y + hitboxOffsetY;

                    // AABB collision check (rectangle-rectangle)
                    const bool hit = bullet.x < playerHitboxX + playerHitboxWidth &&
                                     bullet.x + bullet.width > playerHitboxX &&
                                     bullet.y < playerHitboxY + playerHitboxHeight &&
                                     bullet.y + bullet.height > playerHitboxY;
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