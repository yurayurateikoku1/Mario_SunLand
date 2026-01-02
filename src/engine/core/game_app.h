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

namespace engine::core
{
    class Time;
    class Config;
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
        [[nodiscard]] bool initRenderer();
        [[nodiscard]] bool initCamera();
        [[nodiscard]] bool initInputManager();
    };

}