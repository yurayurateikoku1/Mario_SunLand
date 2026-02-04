#include "context.h"
#include <spdlog/spdlog.h>
engine::core::Context::Context(engine::input::InputManager &input_manager, engine::render::Renderer &render, engine::resource::ResourceManager &resource_manager, engine::render::Camera &camera, engine::render::TextRenderer &text_renderer, engine::physics::PhysicsEngine &physics_engine, engine::audio::AudioPlayer &audio_player, engine::core::GameState &game_state)
    : _input_manager(input_manager), _renderer(render), _resource_manager(resource_manager), _camera(camera), _text_renderer(text_renderer), _physics_engine(physics_engine), _audio_player(audio_player), _game_state(game_state)
{
    spdlog::info("Context created");
}
