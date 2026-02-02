#pragma once
#include "component.h"
#include <string>
#include <unordered_map>

namespace engine::audio
{
    class AudioPlayer;
}

namespace engine::render
{
    class Camera;
}

namespace engine::component
{
    class TransformComponent;
    class AudioComponent : public Component
    {
        friend class engine::object::GameObject;

    private:
        engine::audio::AudioPlayer *_audio_player;
        engine::render::Camera *_camera;
        engine::component::TransformComponent *_transform;
        std::unordered_map<std::string, std::string> _sound_id_to_path;

    public:
        AudioComponent(engine::audio::AudioPlayer *audio_player, engine::render::Camera *camera);
        ~AudioComponent() override = default;
        AudioComponent(const AudioComponent &) = delete;
        AudioComponent(AudioComponent &&) = delete;
        AudioComponent &operator=(const AudioComponent &) = delete;
        AudioComponent &operator=(AudioComponent &&) = delete;

        void playerSound(const std::string &sound_id, int channel = -1, bool use_spatial = false);

        void addSound(const std::string &sound_id, const std::string &sound_path);

    private:
        void init() override;
        void update(float dt, engine::core::Context &) override;
    };
}