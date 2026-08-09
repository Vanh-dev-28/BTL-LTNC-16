#include "Managers/AudioManager.h"
#include <algorithm>

namespace SpaceInvaders
{

    AudioManager &AudioManager::instance()
    {
        static AudioManager manager;
        return manager;
    }

    AudioManager::~AudioManager()
    {
        clear();
    }

    bool AudioManager::initialize()
    {
        if (mixer_ != nullptr)
        {
            return true;
        }

        if (!MIX_Init())
        {
            return false;
        }
        mixerLibraryInitialized_ = true;

        mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (mixer_ == nullptr)
        {
            MIX_Quit();
            mixerLibraryInitialized_ = false;
            return false;
        }

        musicTrack_ = MIX_CreateTrack(mixer_);
        if (musicTrack_ == nullptr)
        {
            MIX_DestroyMixer(mixer_);
            mixer_ = nullptr;
            MIX_Quit();
            mixerLibraryInitialized_ = false;
            return false;
        }

        return true;
    }

    bool AudioManager::playMusic(const std::string &path, int loops)
    {
        if (!initialize())
        {
            return false;
        }

        stopMusic();
        currentMusic_ = MIX_LoadAudio(mixer_, path.c_str(), false);
        if (currentMusic_ == nullptr || !MIX_SetTrackAudio(musicTrack_, currentMusic_))
        {
            MIX_DestroyAudio(currentMusic_);
            currentMusic_ = nullptr;
            return false;
        }

        const SDL_PropertiesID options = SDL_CreateProperties();
        if (options == 0)
        {
            MIX_SetTrackAudio(musicTrack_, nullptr);
            MIX_DestroyAudio(currentMusic_);
            currentMusic_ = nullptr;
            return false;
        }

        const bool configured = SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
        MIX_SetTrackGain(musicTrack_, static_cast<float>(musicVolume_) / 100.0f);
        const bool playing = configured && MIX_PlayTrack(musicTrack_, options);
        SDL_DestroyProperties(options);
        if (!playing)
        {
            MIX_SetTrackAudio(musicTrack_, nullptr);
            MIX_DestroyAudio(currentMusic_);
            currentMusic_ = nullptr;
        }
        return playing;
    }

    bool AudioManager::playSFX(const std::string &path, int loops)
    {
        if (!initialize())
        {
            return false;
        }

        MIX_Audio *sound = nullptr;
        const auto found = soundEffects_.find(path);
        if (found == soundEffects_.end())
        {
            sound = MIX_LoadAudio(mixer_, path.c_str(), true);
            if (sound == nullptr)
            {
                return false;
            }
            soundEffects_.emplace(path, sound);
        }
        else
        {
            sound = found->second;
        }
        if (loops == 0)
        {
            return MIX_PlayAudio(mixer_, sound);
        }

        MIX_Track *track = MIX_CreateTrack(mixer_);
        if (track == nullptr || !MIX_SetTrackAudio(track, sound))
        {
            MIX_DestroyTrack(track);
            return false;
        }

        const SDL_PropertiesID options = SDL_CreateProperties();
        if (options == 0)
        {
            MIX_DestroyTrack(track);
            return false;
        }

        const bool configured = SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
        const bool playing = configured && MIX_PlayTrack(track, options);
        SDL_DestroyProperties(options);
        if (!playing)
        {
            MIX_DestroyTrack(track);
            return false;
        }

        loopingSfxTracks_.push_back(track);
        return true;
    }

    void AudioManager::stopMusic()
    {
        if (musicTrack_ != nullptr)
        {
            MIX_StopTrack(musicTrack_, 0);
            MIX_SetTrackAudio(musicTrack_, nullptr);
        }
        if (currentMusic_ != nullptr)
        {
            MIX_DestroyAudio(currentMusic_);
            currentMusic_ = nullptr;
        }
    }

    void AudioManager::clear()
    {
        stopMusic();
        for (MIX_Track *track : loopingSfxTracks_)
        {
            MIX_DestroyTrack(track);
        }
        loopingSfxTracks_.clear();

        for (const auto &[path, sound] : soundEffects_)
        {
            (void)path;
            MIX_DestroyAudio(sound);
        }
        soundEffects_.clear();

        if (musicTrack_ != nullptr)
        {
            MIX_DestroyTrack(musicTrack_);
            musicTrack_ = nullptr;
        }
        if (mixer_ != nullptr)
        {
            MIX_DestroyMixer(mixer_);
            mixer_ = nullptr;
        }
        if (mixerLibraryInitialized_)
        {
            MIX_Quit();
            mixerLibraryInitialized_ = false;
        }
    }

    void AudioManager::setMusicVolume(int volume)
{
    volume = std::clamp(volume, 0, 100);

    musicVolume_ = volume;

    if (musicTrack_ != nullptr)
    {
        float gain = static_cast<float>(musicVolume_) / 100.0f;
        MIX_SetTrackGain(musicTrack_, gain);
    }
}

} // namespace SpaceInvaders
