#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <SDL3_mixer/SDL_mixer.h>

namespace engine::resource
{

    /// @brief 音频管理器
    class AudioManager
    {
        friend class ResourceManager;

    public:
        AudioManager();
        AudioManager(const AudioManager &) = delete;
        AudioManager &operator=(const AudioManager &) = delete;
        AudioManager(AudioManager &&) = delete;
        AudioManager &operator=(AudioManager &&) = delete;
        ~AudioManager();

    private:
        struct SDLAudioDeleter
        {
            void operator()(MIX_Audio *audio) const
            {
                if (audio)
                {
                    MIX_DestroyAudio(audio);
                }
            }
        };

        struct SDLTrackDeleter
        {
            void operator()(MIX_Track *track) const
            {
                if (track)
                {
                    MIX_DestroyTrack(track);
                }
            }
        };

        MIX_Mixer *_mixer = nullptr;
        std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLAudioDeleter>> _audios;
        std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLAudioDeleter>> _musics;

        // 音乐和音效的专用轨道
        MIX_Track *_musicTrack = nullptr;
        MIX_Track *_soundTrack = nullptr;

        // 当前播放的音频资源
        MIX_Audio *_musicAudio = nullptr;
        MIX_Audio *_soundAudio = nullptr;

    private:
        MIX_Audio *loadSound(const std::string &file_path);
        MIX_Audio *loadMusic(const std::string &file_path);
        void unloadSound(const std::string &file_path);
        void unloadMusic(const std::string &file_path);
        MIX_Audio *getSound(const std::string &file_path);
        MIX_Audio *getMusic(const std::string &file_path);
        void clearSounds();
        void clearMusics();

        /// @brief 播放音效
        /// @param file_path 音频文件路径
        /// @param loops 循环次数 (0=播放一次, -1=无限循环)
        void playSound(const std::string &file_path, int loops = 0);

        /// @brief 播放音乐
        /// @param file_path 音乐文件路径
        /// @param loops 循环次数 (0=播放一次, -1=无限循环)
        void playMusic(const std::string &file_path, int loops = -1);

        /// @brief 停止音乐
        void stopMusic();

        /// @brief 停止音效
        void stopSound();

        /// @brief 暂停音乐
        void pauseMusic();

        /// @brief 暂停音效
        void pauseSound();

        /// @brief 恢复音乐
        void resumeMusic();

        /// @brief 恢复音效
        void resumeSound();

        /// @brief 设置音乐音量
        /// @param gain 音量增益 (0.0-1.0)
        void setMusicGain(float gain);

        /// @brief 设置音效音量
        /// @param gain 音量增益 (0.0-1.0)
        void setSoundGain(float gain);
    };
}