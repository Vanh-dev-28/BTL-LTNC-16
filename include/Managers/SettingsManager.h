#pragma once

#include <string>

namespace SpaceInvaders
{

class SettingsManager
{
public:
    enum class SettingsState
    {
    Normal,
    ResolutionPopup
    };
    static SettingsManager& instance();

    // Music
    int getMusicVolume() const;
    void setMusicVolume(int volume);
    // Resolution
    const std::string& getResolution() const;
    void setResolution(const std::string& resolution);

    void apply();
    void discard();
    bool consumeApplyRequest();

private:
    SettingsManager() = default;

    // Current settings
    std::string currentResolution_ = "1280x720";
    int currentMusicVolume_ = 70;
    bool applyRequested_ = false;

    // Pending settings
    std::string pendingResolution_ = currentResolution_;
    int pendingMusicVolume_ = currentMusicVolume_;
};

}