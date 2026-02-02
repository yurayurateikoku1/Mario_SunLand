#include "audio_manager.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <stdexcept>
engine::resource::AudioManager::AudioManager()
{
    if (!MIX_Init())
    {
        throw std::runtime_error("SDL_mixer Init Failed" + std::string(SDL_GetError()));
        return;
    }
    SDL_AudioSpec spec{};
    spec.freq = 48000;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;
    _mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
}

engine::resource::AudioManager::~AudioManager()
{
    clearSounds();
    clearMusics();

    if (_mixer)
    {
        MIX_DestroyMixer(_mixer);
        _mixer = nullptr;
    }
    MIX_Quit();
}

MIX_Audio *engine::resource::AudioManager::loadSound(const std::string &file_path)
{
    // 检查是否已经加载
    auto it = _audios.find(file_path);
    if (it != _audios.end())
    {
        return it->second.get();
    }

    // 加载音频文件
    MIX_Audio *audio = MIX_LoadAudio(_mixer, file_path.c_str(), false);
    if (!audio)
    {
        spdlog::error("Failed to load sound: {} - {}", file_path, SDL_GetError());
        return nullptr;
    }

    // 存储到容器中
    _audios[file_path] = std::unique_ptr<MIX_Audio, SDLAudioDeleter>(audio);
    spdlog::info("Loaded sound: {}", file_path);
    return audio;
}

MIX_Audio *engine::resource::AudioManager::loadMusic(const std::string &file_path)
{
    // 检查是否已经加载
    auto it = _musics.find(file_path);
    if (it != _musics.end())
    {
        return it->second.get();
    }

    // 加载音乐文件 (预解码以减少播放时的 CPU 负载)
    MIX_Audio *music = MIX_LoadAudio(_mixer, file_path.c_str(), true);
    if (!music)
    {
        spdlog::error("Failed to load music: {} - {}", file_path, SDL_GetError());
        return nullptr;
    }

    // 存储到容器中
    _musics[file_path] = std::unique_ptr<MIX_Audio, SDLAudioDeleter>(music);
    spdlog::info("Loaded music: {}", file_path);
    return music;
}

void engine::resource::AudioManager::unloadSound(const std::string &file_path)
{
    auto it = _audios.find(file_path);
    if (it != _audios.end())
    {
        _audios.erase(it);
        spdlog::info("Unloaded sound: {}", file_path);
    }
}

void engine::resource::AudioManager::unloadMusic(const std::string &file_path)
{
    auto it = _musics.find(file_path);
    if (it != _musics.end())
    {
        _musics.erase(it);
        spdlog::info("Unloaded music: {}", file_path);
    }
}

MIX_Audio *engine::resource::AudioManager::getSound(const std::string &file_path)
{
    auto it = _audios.find(file_path);
    if (it != _audios.end())
    {
        return it->second.get();
    }
    return nullptr;
}

MIX_Audio *engine::resource::AudioManager::getMusic(const std::string &file_path)
{
    auto it = _musics.find(file_path);
    if (it != _musics.end())
    {
        return it->second.get();
    }
    return nullptr;
}

void engine::resource::AudioManager::clearSounds()
{
    _audios.clear();
    spdlog::info("Cleared all sounds");
}

void engine::resource::AudioManager::clearMusics()
{
    _musics.clear();
    spdlog::info("Cleared all music");
}
