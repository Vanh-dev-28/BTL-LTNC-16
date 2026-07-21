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

    // SFX
    int getSFXVolume() const;
    void setSFXVolume(int volume);

    // Fullscreen
    bool isFullscreen() const;
    void setFullscreen(bool fullscreen);

    // Resolution
    const std::string& getResolution() const;
    void setResolution(const std::string& resolution);

    void apply();
    void discard();


private:
    SettingsManager() = default;

// Current settings
std::string currentResolution_ = "1280x720";
bool currentFullscreen_ = false;

int currentMusicVolume_ = 80;
int currentSFXVolume_ = 100;

// Pending settings
std::string pendingResolution_ = currentResolution_;
bool pendingFullscreen_ = currentFullscreen_;

int pendingMusicVolume_ = currentMusicVolume_;
int pendingSFXVolume_ = currentSFXVolume_;
};

}