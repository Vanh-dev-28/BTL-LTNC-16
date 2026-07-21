#include "Managers/SettingsManager.h"

namespace SpaceInvaders
{

SettingsManager& SettingsManager::instance()
{
    static SettingsManager manager;
    return manager;
}

//======================
// Music
//======================

int SettingsManager::getMusicVolume() const
{
    return pendingMusicVolume_;
}

void SettingsManager::setMusicVolume(int volume)
{
    pendingMusicVolume_ = volume;
}

//======================
// SFX
//======================

int SettingsManager::getSFXVolume() const
{
    return pendingSFXVolume_;
}

void SettingsManager::setSFXVolume(int volume)
{
    pendingSFXVolume_ = volume;
}

//======================
// Fullscreen
//======================

bool SettingsManager::isFullscreen() const
{
    return pendingFullscreen_;
}

void SettingsManager::setFullscreen(bool fullscreen)
{
    pendingFullscreen_ = fullscreen;
}

//======================
// Resolution
//======================

const std::string& SettingsManager::getResolution() const
{
    return pendingResolution_;
}

void SettingsManager::setResolution(const std::string& resolution)
{
    pendingResolution_ = resolution;
}

void SettingsManager::apply()
{
    currentResolution_ = pendingResolution_;
    currentFullscreen_ = pendingFullscreen_;

    currentMusicVolume_ = pendingMusicVolume_;
    currentSFXVolume_ = pendingSFXVolume_;
}

void SettingsManager::discard()
{
    pendingResolution_ = currentResolution_;
    pendingFullscreen_ = currentFullscreen_;

    pendingMusicVolume_ = currentMusicVolume_;
    pendingSFXVolume_ = currentSFXVolume_;
}

}