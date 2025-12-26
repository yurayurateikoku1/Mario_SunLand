#pragma once
#include <memory>
struct SDL_Window;
struct SDL_Renderer;
struct MIX_Mixer;
namespace engine::core
{
    class Time;
    /// @brief 主应用程序,初始化SDL,运行主循环
    class GameApp final
    {
    private:
        SDL_Window *_window{nullptr};
        SDL_Renderer *_sdl_renderer{nullptr};
        MIX_Mixer *_mixer = nullptr;
        bool _is_running{nullptr};

        std::unique_ptr<engine::core::Time> _time{nullptr};

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
    };

}