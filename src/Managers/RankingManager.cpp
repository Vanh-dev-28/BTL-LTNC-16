#include "Managers/RankingManager.h"

#include <fstream>
#include <algorithm>

namespace SpaceInvaders
{
    RankingManager& RankingManager::instance()
    {
        static RankingManager instance;
        return instance;
    }
    
    const std::vector<ScoreEntry>& RankingManager::getScores() const
    {
        return scores_;
    }

    bool RankingManager::load(const std::string& path)
    {
        scores_.clear();
        std::ifstream file(path);
        if (!file.is_open())
        {
            return false;
        }

        std::string line;
        while (std::getline(file, line))
        {
            size_t separator = line.find('|');
            if (separator == std::string::npos)
                continue;
            ScoreEntry entry;
            entry.name = line.substr(0, separator);
            entry.score = std::stoi(line.substr(separator + 1));
            scores_.push_back(entry);
        }

        file.close();
        std::sort(scores_.begin(), scores_.end(), [](const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; } );
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
            file << entry.name << "|" << entry.score << "\n";
        }
        file.close();
        return true;
    }

    void RankingManager::addScore(const std::string& name, int score)
    {
        scores_.push_back({name, score});
        std::sort(scores_.begin(), scores_.end(), [](const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; } );
    }
}