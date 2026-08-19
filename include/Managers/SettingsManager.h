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
        int getMusicVolume() const;
        void setMusicVolume(int volume);
        const std::string& getResolution() const;
        void setResolution(const std::string& resolution);
        void apply();
        void discard();
        bool consumeApplyRequest();
    private:
        SettingsManager() = default;
        std::string currentResolution_ = "1280x720";
        int currentMusicVolume_ = 70;
        bool applyRequested_ = false;
        std::string pendingResolution_ = currentResolution_;
        int pendingMusicVolume_ = currentMusicVolume_;
    };
}