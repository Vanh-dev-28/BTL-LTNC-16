#include "Managers/AudioManager.h"

namespace SpaceInvaders {

AudioManager& AudioManager::instance()
{
    static AudioManager manager;
    return manager;
}

AudioManager::~AudioManager()
{
    if (Mix_QuerySpec(nullptr, nullptr, nullptr) != 0) {
        clear();
    }
}

bool AudioManager::playMusic(const std::string& path, int loops)
{
    stopMusic();
    currentMusic_ = Mix_LoadMUS(path.c_str());
    return currentMusic_ != nullptr && Mix_PlayMusic(currentMusic_, loops) == 0;
}

bool AudioManager::playSFX(const std::string& path, int loops)
{
    Mix_Chunk* sound = nullptr;
    const auto found = soundEffects_.find(path);
    if (found == soundEffects_.end()) {
        sound = Mix_LoadWAV(path.c_str());
        if (sound == nullptr) {
            return false;
        }
        soundEffects_.emplace(path, sound);
    } else {
        sound = found->second;
    }
    return Mix_PlayChannel(-1, sound, loops) != -1;
}

void AudioManager::stopMusic()
{
    Mix_HaltMusic();
    if (currentMusic_ != nullptr) {
        Mix_FreeMusic(currentMusic_);
        currentMusic_ = nullptr;
    }
}

void AudioManager::clear()
{
    stopMusic();
    for (const auto& [path, sound] : soundEffects_) {
        (void)path;
        Mix_FreeChunk(sound);
    }
    soundEffects_.clear();
}

} // namespace SpaceInvaders
