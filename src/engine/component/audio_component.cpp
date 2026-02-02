#include "audio_component.h"
#include "transform_component.h"
#include "../object/game_object.h"
#include "../audio/audio_player.h"
#include "../render/camera.h"
#include <spdlog/spdlog.h>
engine::component::AudioComponent::AudioComponent(engine::audio::AudioPlayer *audio_player, engine::render::Camera *camera)
    : _audio_player(audio_player), _camera(camera)
{
    if (!_audio_player || !_camera)
    {
        spdlog::error("AudioComponent init failed.");
    }
}

void engine::component::AudioComponent::playerSound(const std::string &sound_id, int channel, bool use_spatial)
{
    auto sound_path = _sound_id_to_path.find(sound_id) != _sound_id_to_path.end() ? _sound_id_to_path[sound_id] : sound_id;
    if (use_spatial && _transform)
    {
        auto camera_center = _camera->getPosition() + _camera->getViewportSize() / 2.0f;
        auto object_position = _transform->getPosition();
        float distance = glm::length(camera_center - object_position);
        if (distance > 150.0f)
        {
            return;
        }
        _audio_player->playSound(sound_path, channel);
    }
    else
    {
        _audio_player->playSound(sound_path, channel);
    }
}

void engine::component::AudioComponent::addSound(const std::string &sound_id, const std::string &sound_path)
{
    if (_sound_id_to_path.find(sound_id) != _sound_id_to_path.end())
    {
    }
    _sound_id_to_path[sound_id] = sound_path;
}

void engine::component::AudioComponent::init()
{
    if (!_owner)
    {
        return;
    }
    _transform = _owner->getComponent<engine::component::TransformComponent>();
    if (!_transform)
    {
        spdlog::warn("GameObject {} does not have a TransformComponent.", _owner->getName());
    }
}

void engine::component::AudioComponent::update(float dt, engine::core::Context &)
{
}
