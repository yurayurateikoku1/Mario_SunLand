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

        MIX_Mixer *getMixer() const { return _mixer; }

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

        MIX_Mixer *_mixer = nullptr;
        std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLAudioDeleter>> _audios;
        std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLAudioDeleter>> _musics;

    private:
        MIX_Audio *loadSound(const std::string &file_path);
        MIX_Audio *loadMusic(const std::string &file_path);
        void unloadSound(const std::string &file_path);
        void unloadMusic(const std::string &file_path);
        MIX_Audio *getSound(const std::string &file_path);
        MIX_Audio *getMusic(const std::string &file_path);
        void clearSounds();
        void clearMusics();
    };
}