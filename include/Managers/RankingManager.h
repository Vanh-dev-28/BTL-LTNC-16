#pragma once

#include <string>
#include <vector>

namespace SpaceInvaders
{

struct ScoreEntry
{
    std::string name;
    int score;
};

class RankingManager
{
public:
    static RankingManager& instance();

    bool load(const std::string& path);
    bool save(const std::string& path);

    void addScore(const std::string& name, int score);

    const std::vector<ScoreEntry>& getScores() const;

private:
    RankingManager() = default;

    std::vector<ScoreEntry> scores_;
};

}