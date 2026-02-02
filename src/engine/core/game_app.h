#pragma once
#include <memory>
struct SDL_Window;
struct SDL_Renderer;
struct MIX_Mixer;
namespace engine::resource
{
    class ResourceManager;
}
namespace engine::render
{
    class Renderer;
    class Camera;
}
namespace engine::input
{
    class InputManager;
} // namespace engine::input

namespace engine::physics
{
    class PhysicsEngine;
}
namespace engine::scene
{
    class SceneManager;
}
namespace engine::audio
{
    class AudioPlayer;
}
namespace engine::core
{
    class Time;
    class Config;
    class Context;
    /// @brief 主应用程序,初始化SDL,运行主循环
    class GameApp final
    {
    private:
        SDL_Window *_window{nullptr};
        SDL_Renderer *_sdl_renderer{nullptr};
        MIX_Mixer *_mixer = nullptr;
        bool _is_running{false};

        std::unique_ptr<engine::core::Time> _time{nullptr};
        std::unique_ptr<engine::resource::ResourceManager> _resource_manager{nullptr};
        std::unique_ptr<engine::render::Renderer> _renderer{nullptr};
        std::unique_ptr<engine::render::Camera> _camera{nullptr};
        std::unique_ptr<engine::core::Config> _config{nullptr};
        std::unique_ptr<engine::input::InputManager> _input_manager{nullptr};
        std::unique_ptr<engine::core::Context> _context{nullptr};
        std::unique_ptr<engine::scene::SceneManager> _scene_manager{nullptr};
        std::unique_ptr<engine::physics::PhysicsEngine> _physics_engine{nullptr};
        std::unique_ptr<engine::audio::AudioPlayer> _audio_player{nullptr};

    public:
        GameApp();
        ~GameApp();
        GameApp(const GameApp &) = delete;
        GameApp(GameApp &&) = delete;
        GameApp &operator=(const GameApp &) = delete;
        GameApp &operator=(GameApp &&) = delete;

        [[nodiscard]] bool init();
        void handleEvents();
        void update(float dt);
        void render();
        void close();

        void run();

        [[nodiscard]] bool initConfig();
        [[nodiscard]] bool initSDL();
        [[nodiscard]] bool initTime();
        [[nodiscard]] bool initResourceManager();
        [[nodiscard]] bool initAudioPlayer();
        [[nodiscard]] bool initRenderer();
        [[nodiscard]] bool initCamera();
        [[nodiscard]] bool initInputManager();
        [[nodiscard]] bool initPhysicsEngine();
        [[nodiscard]] bool initContext();
        [[nodiscard]] bool initSceneManager();
    };

}