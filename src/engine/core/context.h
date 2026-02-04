#pragma once
namespace engine::input
{
    class InputManager;
}
namespace engine::render
{
    class Renderer;
    class Camera;
    class TextRenderer;
}

namespace engine::resource
{
    class ResourceManager;
}

namespace engine::physics
{
    class PhysicsEngine;
}

namespace engine::audio
{
    class AudioPlayer;
}
namespace engine::core
{
    class GameState;
    /// @brief 持有对核心引擎模块引用的上下文对象
    /// 简化依赖注入，传递Context来获取引擎的各个模块
    class Context final
    {
    private:
        /// @brief 输入管理器
        engine::input::InputManager &_input_manager;
        /// @brief 渲染器
        engine::render::Renderer &_renderer;
        /// @brief 资源管理器
        engine::resource::ResourceManager &_resource_manager;
        /// @brief 摄像机
        engine::render::Camera &_camera;
        /// @brief 物理引擎
        engine::physics::PhysicsEngine &_physics_engine;
        engine::audio::AudioPlayer &_audio_player;
        engine::render::TextRenderer &_text_renderer;
        engine::core::GameState &_game_state;

    public:
        Context(engine::input::InputManager &input_manager,
                engine::render::Renderer &render,
                engine::resource::ResourceManager &resource_manager,
                engine::render::Camera &camera,
                engine::render::TextRenderer &text_renderer,
                engine::physics::PhysicsEngine &physics_engine,
                engine::audio::AudioPlayer &audio_player,
                engine::core::GameState &game_state);
        Context(const Context &) = delete;
        Context(Context &&) = delete;
        Context &operator=(const Context &) = delete;
        Context &operator=(Context &&) = delete;

        engine::input::InputManager &getInputManager() const { return _input_manager; }
        engine::render::Renderer &getRender() const { return _renderer; }
        engine::resource::ResourceManager &getResourceManager() const { return _resource_manager; }
        engine::render::Camera &getCamera() const { return _camera; }
        engine::render::TextRenderer &getTextRenderer() const { return _text_renderer; }
        engine::physics::PhysicsEngine &getPhysicsEngine() const { return _physics_engine; }
        engine::audio::AudioPlayer &getAudioPlayer() const { return _audio_player; }
        engine::core::GameState &getGameState() const { return _game_state; }
    };
}