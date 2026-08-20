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

        bullets_.erase(
            std::remove_if(bullets_.begin(), bullets_.end(), [](const Bullet &b)
                           { return !b.active; }),
            bullets_.end());
    }

    void GameScene::updateEnemies(float deltaTime)
    {
        if (!pendingEnemies_.empty())
        {
            spawnTimer_ -= deltaTime;
            if (spawnTimer_ <= 0.0f)
            {
                const auto &spawnData = pendingEnemies_.front();
                enemies_.emplace_back(
                    spawnData.type,
                    spawnData.movePattern,
                    spawnData.entryPattern,
                    spawnData.startPos,
                    spawnData.targetPos,
                    spawnData.speed,
                    spawnData.c1,
                    spawnData.c2);

                spawnTimer_ = spawnData.spawnDelay;

                pendingEnemies_.erase(pendingEnemies_.begin());
            }
        }

        m_swarmSharedTime += deltaTime;

        const float SCALE_SPEED = 0.2f;
        const float MIN_SCALE = 0.8f;
        const float MAX_SCALE = 1.2f;
        m_formationScale += m_formationScaleDirection * SCALE_SPEED * deltaTime;
        if (m_formationScale > MAX_SCALE || m_formationScale < MIN_SCALE)
        {
            m_formationScaleDirection *= -1.0f;
            m_formationScale = std::clamp(m_formationScale, MIN_SCALE, MAX_SCALE);
        }

        m_diveAttackTimer -= deltaTime;
        if (m_diveAttackTimer <= 0.0f)
        {
            if (currentWave_ >= 3)
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
                    int divers = (currentWave_ >= 4) ? 2 : 1;

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

            if (currentWave_ == 5)
            {
                m_diveAttackTimer = 2.0f + (static_cast<float>(rand()) / RAND_MAX) * 1.5f;
            }
            else
            {
                m_diveAttackTimer = 3.5f + (static_cast<float>(rand()) / RAND_MAX) * 3.0f;
            }
        }

        bool hitEdge = false;
        bool hasActiveEnemy = false;
        float minX = Constants::SCREEN_WIDTH;
        float maxX = 0.0f;

        for (auto &enemy : enemies_)
        {
            if (enemy.alive)
            {
                Vector2 swarmVelocity = {0, 0};
                if (enemy.getState() == EnemyState::Active)
                {
                    swarmVelocity.x = enemyDirection_ * enemy.getSpeed() * 0.5f;

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
                    hasActiveEnemy = true;
                    if (enemy.x < minX)
                        minX = enemy.x;
                    if (enemy.x + enemy.width > maxX)
                        maxX = enemy.x + enemy.width;
                }

                if (enemy.getState() == EnemyState::Diving && (rand() % 150 == 0))
                {
                    const float bulletWidth = 8.0f;
                    float bulletX = enemy.x + (enemy.width - bulletWidth) / 2.0f;
                    bullets_.emplace_back(bulletX, enemy.y + enemy.height, 350.0f, BulletOwner::Enemy);
                }
            }
        }

        static float edgeCooldown = 0.0f;
        edgeCooldown -= deltaTime;

        if (hasActiveEnemy && edgeCooldown <= 0.0f)
        {
            if (minX <= 30.0f && enemyDirection_ < 0.0f)
            {
                hitEdge = true;
            }
            else if (maxX >= Constants::SCREEN_WIDTH - 30.0f && enemyDirection_ > 0.0f)
            {
                hitEdge = true;
            }
        }

        if (hitEdge)
        {
            enemyDirection_ *= -1.0f;
            edgeCooldown = 0.5f;
            for (auto &enemy : enemies_)
            {
                if (enemy.alive && enemy.getState() == EnemyState::Active)
                {
                    enemy.y += 6.0f;
                }
            }
            m_formationCenter.y += 6.0f;
        }

        for (const auto &enemy : enemies_)
        {
            if (enemy.alive &&
                enemy.getState() == EnemyState::Active &&
                std::abs(enemy.y - enemy.getTargetPosition().y) < 15.0f &&
                (enemy.y + enemy.height >= Constants::SCREEN_HEIGHT - 30.0f))
            {
                gameOver_ = true;
                playerWon_ = false;
                break;
            }
        }

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
                for (auto &enemy : enemies_)
                {
                    if (!enemy.alive)
                    {
                        continue;
                    }

                    const bool hit = bullet.x < enemy.x + enemy.width &&
                                     bullet.x + bullet.width > enemy.x &&
                                     bullet.y < enemy.y + enemy.height &&
                                     bullet.y + bullet.height > enemy.y;
                    if (hit)
                    {
                        const float damage = (bullet.type == BulletType::Fireball) ? 2.0f : 1.0f;
                        enemy.takeDamage(damage);

                        if (!enemy.alive)
                        {
                            score_ += 10;
                            spawnPowerUp(enemy.x + enemy.width / 2.0f - 24.0f, enemy.y);
                        }

                        if (bullet.type != BulletType::Fireball)
                        {
                            bullet.active = false;
                            break;
                        }
                    }
                }
            }
            else
            {
                if (player_.isAlive())
                {
                    if (player_.isShieldActive())
                    {
                        const SDL_FRect shieldRect = getPlayerShieldHitbox();
                        const float shieldCenterX = shieldRect.x + shieldRect.w / 2.0f;
                        const float shieldCenterY = shieldRect.y + shieldRect.h / 2.0f;
                        const float shieldRadius = shieldRect.w / 2.0f;

                        const SDL_FRect laserHitbox = getEnemyLaserHitbox(bullet);

                        const float closestX = std::clamp(shieldCenterX, laserHitbox.x, laserHitbox.x + laserHitbox.w);
                        const float closestY = std::clamp(shieldCenterY, laserHitbox.y, laserHitbox.y + laserHitbox.h);

                        const float distanceX = shieldCenterX - closestX;
                        const float distanceY = shieldCenterY - closestY;
                        const float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

                        const bool shieldHit = distanceSquared < (shieldRadius * shieldRadius);

                        if (shieldHit)
                        {
                            bullet.active = false;
                            continue;
                        }
                    }

                    const SDL_FRect playerHitbox = getPlayerHitbox();

                    const SDL_FRect laserHitbox = getEnemyLaserHitbox(bullet);

                    const bool hit = bullet.active &&
                                     (laserHitbox.x < playerHitbox.x + playerHitbox.w) &&
                                     (laserHitbox.x + laserHitbox.w > playerHitbox.x) &&
                                     (laserHitbox.y < playerHitbox.y + playerHitbox.h) &&
                                     (laserHitbox.y + laserHitbox.h > playerHitbox.y);

                    if (hit)
                    {
                        player_.takeDamage(Constants::ENEMY_LASER_DAMAGE);
                        bullet.active = false;
                    }
                }
            }
        }

        if (player_.isAlive())
        {
            const SDL_FRect playerHitbox = getPlayerHitbox();

            for (auto &enemy : enemies_)
            {
                if (!enemy.alive)
                {
                    continue;
                }

                const bool hit = playerHitbox.x < enemy.x + enemy.width &&
                                 playerHitbox.x + playerHitbox.w > enemy.x &&
                                 playerHitbox.y < enemy.y + enemy.height &&
                                 playerHitbox.y + playerHitbox.h > enemy.y;

                if (hit)
                {
                    if (player_.isShieldActive())
                    {
                        enemy.takeDamage(999.0f);
                    }
                    else
                    {
                        player_.takeDamage(50.0f);
                        enemy.takeDamage(999.0f);
                    }

                    if (!enemy.alive)
                    {
                        score_ += 10;
                        spawnPowerUp(enemy.x + enemy.width / 2.0f - 24.0f, enemy.y);
                    }
                }
            }
        }
    }
}