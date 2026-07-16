#include "Managers/RankingManager.h"

#include <fstream>

namespace SpaceInvaders
{

RankingManager& RankingManager::instance()
{
    static RankingManager instance;
    return instance;
}

bool RankingManager::load(const std::string& path)
{
    scores_.clear();

    std::ifstream file(path);

    if (!file.is_open())
    {
        return false;
    }

    ScoreEntry entry;

    while (file >> entry.name >> entry.score)
    {
        scores_.push_back(entry);
    }

    file.close();

    return true;
}

bool RankingManager::save(const std::string& path)
{
    std::ofstream file(path);

    if (!file.is_open())
    {
        return false;
    }

    for (const auto& entry : scores_)
    {
        file << entry.name << " "
             << entry.score << "\n";
    }

    file.close();

    return true;
}

void RankingManager::addScore(const std::string& name, int score)
{
    scores_.push_back({name, score});
}

const std::vector<ScoreEntry>& RankingManager::getScores() const
{
    return scores_;
}

}