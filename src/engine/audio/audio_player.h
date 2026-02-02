#pragma once
#include <string>
struct MIX_Track;
struct MIX_Audio;
struct MIX_Mixer;
namespace engine::resource
{
    class ResourceManager;
}
namespace engine::audio
{
    class AudioPlayer final
    {
    private:
        engine::resource::ResourceManager *_resource_manager;
        MIX_Mixer *_mixer = nullptr;

        MIX_Track *_musicTrack = nullptr;
        MIX_Track *_soundTrack = nullptr;

        MIX_Audio *_musicAudio = nullptr;
        MIX_Audio *_soundAudio = nullptr;

        std::string _current_music;

    public:
        explicit AudioPlayer(engine::resource::ResourceManager *resource_manager);
        ~AudioPlayer();

        AudioPlayer(const AudioPlayer &) = delete;
        AudioPlayer(AudioPlayer &&) = delete;
        AudioPlayer &operator=(const AudioPlayer &) = delete;
        AudioPlayer &operator=(AudioPlayer &&) = delete;

        int playSound(const std::string &sound_path, int channel = -1);

        int playMusic(const std::string &music_path, int loops = -1, int fade_in_ms = 0);

        void stopMusic(int fade_out_ms = 0);

        void pauseMusic();

        void resumeMusic();

        void setSoundVolume(float volume, int channel = -1);

        void setMusicVolume(float volume);

        float getMusicVolume();

        float getSoundVolume(int channel = -1);
    };
}
