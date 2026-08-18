#pragma once

#include <SDL3_mixer/SDL_mixer.h>
#include <unordered_map>
#include <string>

namespace SpaceInvaders
{

    /** Provides music and sound-effect playback services. */
    class AudioManager
    {
    public:
        static AudioManager &instance();

        /** Initializes SDL_mixer and opens the default playback device. */
        bool initialize();
        bool playMusic(const std::string &path, int loops = -1);
        bool playSFX(const std::string& path);
        void setMusicVolume(int volume);
        void stopMusic();
        void clear();

    private:
        AudioManager() = default;
        ~AudioManager();

        AudioManager(const AudioManager &) = delete;
        AudioManager &operator=(const AudioManager &) = delete;

        MIX_Mixer *mixer_{nullptr};
        MIX_Track *musicTrack_{nullptr};
        MIX_Track *sfxTrack_{nullptr};
        bool mixerLibraryInitialized_{false};
        MIX_Audio *currentMusic_{nullptr};
        std::string currentMusicPath_;
        int musicVolume_ = 100;
        int sfxVolume_ = 50;
        std::unordered_map<std::string, MIX_Audio*> soundEffects_;

    };

} // namespace SpaceInvaders
