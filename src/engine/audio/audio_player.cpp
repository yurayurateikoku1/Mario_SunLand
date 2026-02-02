#include "audio_player.h"
#include "../resource/resource_manager.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

engine::audio::AudioPlayer::AudioPlayer(engine::resource::ResourceManager *resource_manager)
    : _resource_manager(resource_manager)
{
    if (!_resource_manager)
    {
        throw std::runtime_error("ResourceManager is nullptr");
    }
    _mixer = _resource_manager->getMixer();
    if (!_mixer)
    {
        throw std::runtime_error("MIX_Mixer is nullptr");
    }
}

engine::audio::AudioPlayer::~AudioPlayer()
{
    if (_musicTrack)
    {
        MIX_StopTrack(_musicTrack, 0);
        MIX_DestroyTrack(_musicTrack);
        _musicTrack = nullptr;
    }
    if (_soundTrack)
    {
        MIX_StopTrack(_soundTrack, 0);
        MIX_DestroyTrack(_soundTrack);
        _soundTrack = nullptr;
    }
    if (_musicAudio)
    {
        MIX_DestroyAudio(_musicAudio);
        _musicAudio = nullptr;
    }
    if (_soundAudio)
    {
        MIX_DestroyAudio(_soundAudio);
        _soundAudio = nullptr;
    }
}

int engine::audio::AudioPlayer::playSound(const std::string &sound_path, int channel)
{
    MIX_Audio *audio = _resource_manager->loadSound(sound_path);
    if (!audio)
    {
        return -1;
    }

    if (!_soundTrack)
    {
        _soundTrack = MIX_CreateTrack(_mixer);
        if (!_soundTrack)
        {
            spdlog::error("Failed to create sound track: {}", SDL_GetError());
            return -1;
        }
    }

    if (_soundAudio)
    {
        MIX_DestroyAudio(_soundAudio);
        _soundAudio = nullptr;
    }

    _soundAudio = MIX_LoadAudio(_mixer, sound_path.c_str(), false);
    if (!_soundAudio)
    {
        spdlog::error("Failed to reload sound audio: {}", SDL_GetError());
        return -1;
    }

    if (!MIX_SetTrackAudio(_soundTrack, _soundAudio))
    {
        spdlog::error("Failed to set track audio: {}", SDL_GetError());
        return -1;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, 0);

    if (!MIX_PlayTrack(_soundTrack, props))
    {
        spdlog::error("Failed to play sound track: {}", SDL_GetError());
        SDL_DestroyProperties(props);
        return -1;
    }

    SDL_DestroyProperties(props);
    spdlog::info("Playing sound: {}", sound_path);
    return 0;
}

int engine::audio::AudioPlayer::playMusic(const std::string &music_path, int loops, int fade_in_ms)
{
    if (music_path == _current_music)
    {
        return true;
    }
    _current_music = music_path;
    if (!_musicTrack)
    {
        _musicTrack = MIX_CreateTrack(_mixer);
        if (!_musicTrack)
        {
            spdlog::error("Failed to create music track: {}", SDL_GetError());
            return -1;
        }
    }

    if (_musicAudio)
    {
        MIX_DestroyAudio(_musicAudio);
        _musicAudio = nullptr;
    }

    _musicAudio = MIX_LoadAudio(_mixer, music_path.c_str(), true);
    if (!_musicAudio)
    {
        spdlog::error("Failed to load music: {}", SDL_GetError());
        return -1;
    }

    if (!MIX_SetTrackAudio(_musicTrack, _musicAudio))
    {
        spdlog::error("Failed to set track audio: {}", SDL_GetError());
        return -1;
    }

    MIX_SetTrackGain(_musicTrack, 0.05f);

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
    if (fade_in_ms > 0)
    {
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fade_in_ms);
    }

    if (!MIX_PlayTrack(_musicTrack, props))
    {
        spdlog::error("Failed to play music track: {}", SDL_GetError());
        SDL_DestroyProperties(props);
        return -1;
    }

    SDL_DestroyProperties(props);
    _current_music = music_path;
    spdlog::info("Playing music: {}", music_path);
    return 0;
}

void engine::audio::AudioPlayer::stopMusic(int fade_out_ms)
{
    if (_musicTrack)
    {
        MIX_StopTrack(_musicTrack, fade_out_ms);
    }
}

void engine::audio::AudioPlayer::pauseMusic()
{
    if (_musicTrack)
    {
        MIX_PauseTrack(_musicTrack);
    }
}

void engine::audio::AudioPlayer::resumeMusic()
{
    if (_musicTrack)
    {
        MIX_ResumeTrack(_musicTrack);
    }
}

void engine::audio::AudioPlayer::setSoundVolume(float volume, int channel)
{
    if (_soundTrack)
    {
        MIX_SetTrackGain(_soundTrack, volume);
    }
}

void engine::audio::AudioPlayer::setMusicVolume(float volume)
{
    if (_musicTrack)
    {
        MIX_SetTrackGain(_musicTrack, volume);
    }
}

float engine::audio::AudioPlayer::getMusicVolume()
{
    if (_musicTrack)
    {
        return MIX_GetTrackGain(_musicTrack);
    }
    return 0.0f;
}

float engine::audio::AudioPlayer::getSoundVolume(int channel)
{
    if (_soundTrack)
    {
        return MIX_GetTrackGain(_soundTrack);
    }
    return 0.0f;
}
