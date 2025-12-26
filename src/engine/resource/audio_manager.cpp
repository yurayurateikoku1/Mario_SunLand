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
    // 停止并销毁音轨
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

    // 销毁音频资源
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

    // 清理缓存的音频资源
    clearSounds();
    clearMusics();

    // 销毁混音器
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

void engine::resource::AudioManager::playSound(const std::string &file_path, int loops)
{
    // 加载音频(如果尚未加载)
    MIX_Audio *audio = loadSound(file_path);
    if (!audio)
    {
        return;
    }

    // 如果音效轨道不存在,创建一个
    if (!_soundTrack)
    {
        _soundTrack = MIX_CreateTrack(_mixer);
        if (!_soundTrack)
        {
            spdlog::error("Failed to create sound track: {}", SDL_GetError());
            return;
        }
    }

    // 销毁之前的音频(如果有)
    if (_soundAudio)
    {
        MIX_DestroyAudio(_soundAudio);
        _soundAudio = nullptr;
    }

    // 重新加载音频
    _soundAudio = MIX_LoadAudio(_mixer, file_path.c_str(), false);
    if (!_soundAudio)
    {
        spdlog::error("Failed to reload sound audio: {}", SDL_GetError());
        return;
    }

    // 设置音频到音轨
    if (!MIX_SetTrackAudio(_soundTrack, _soundAudio))
    {
        spdlog::error("Failed to set track audio: {}", SDL_GetError());
        return;
    }

    // 创建属性并设置循环次数
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);

    // 开始播放
    if (!MIX_PlayTrack(_soundTrack, props))
    {
        spdlog::error("Failed to play track: {}", SDL_GetError());
        SDL_DestroyProperties(props);
        return;
    }

    SDL_DestroyProperties(props);
    spdlog::info("Playing sound: {}", file_path);
}

void engine::resource::AudioManager::playMusic(const std::string &file_path, int loops)
{
    // 如果音乐轨道不存在,创建一个
    if (!_musicTrack)
    {
        _musicTrack = MIX_CreateTrack(_mixer);
        if (!_musicTrack)
        {
            spdlog::error("Failed to create music track: {}", SDL_GetError());
            return;
        }
    }

    // 销毁之前的音频(如果有)
    if (_musicAudio)
    {
        MIX_DestroyAudio(_musicAudio);
        _musicAudio = nullptr;
    }

    // 加载新的音乐文件(预解码以减少播放时的 CPU 负载)
    _musicAudio = MIX_LoadAudio(_mixer, file_path.c_str(), true);
    if (!_musicAudio)
    {
        spdlog::error("Failed to load music: {}", SDL_GetError());
        return;
    }

    // 设置音频到音轨
    if (!MIX_SetTrackAudio(_musicTrack, _musicAudio))
    {
        spdlog::error("Failed to set track audio: {}", SDL_GetError());
        return;
    }

    // 设置音量
    MIX_SetTrackGain(_musicTrack, 0.05f);

    // 创建属性并设置循环次数
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);

    // 开始播放
    if (!MIX_PlayTrack(_musicTrack, props))
    {
        spdlog::error("Failed to play track: {}", SDL_GetError());
        SDL_DestroyProperties(props);
        return;
    }

    SDL_DestroyProperties(props);
    spdlog::info("Playing music: {}", file_path);
}

void engine::resource::AudioManager::stopMusic()
{
    if (_musicTrack)
    {
        MIX_StopTrack(_musicTrack, 0);
    }
}

void engine::resource::AudioManager::stopSound()
{
    if (_soundTrack)
    {
        MIX_StopTrack(_soundTrack, 0);
    }
}

void engine::resource::AudioManager::pauseMusic()
{
    if (_musicTrack)
    {
        MIX_PauseTrack(_musicTrack);
    }
}

void engine::resource::AudioManager::pauseSound()
{
    if (_soundTrack)
    {
        MIX_PauseTrack(_soundTrack);
    }
}

void engine::resource::AudioManager::resumeMusic()
{
    if (_musicTrack)
    {
        MIX_ResumeTrack(_musicTrack);
    }
}

void engine::resource::AudioManager::resumeSound()
{
    if (_soundTrack)
    {
        MIX_ResumeTrack(_soundTrack);
    }
}

void engine::resource::AudioManager::setMusicGain(float gain)
{
    if (_musicTrack)
    {
        MIX_SetTrackGain(_musicTrack, gain);
    }
}

void engine::resource::AudioManager::setSoundGain(float gain)
{
    if (_soundTrack)
    {
        MIX_SetTrackGain(_soundTrack, gain);
    }
}
