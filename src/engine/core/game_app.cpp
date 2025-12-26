#include "game_app.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include "time.h"
namespace engine::core
{
    GameApp::GameApp()
        : _time(std::make_unique<Time>())
    {
    }
    GameApp::~GameApp()
    {
        if (_is_running)
        {
            spdlog::warn("GameApp is still running,now is shutting down");
        }
        close();
    }

    void GameApp::run()
    {
        if (!init())
        {
            spdlog::error("GameApp init failed,now is shutting down");
            return;
        }
        _time->setTargetFps(144);
        while (_is_running)
        {
            _time->update();
            float dt = _time->getDeltaTime();

            handleEvents();
            update(dt);
            render();
        }
        close();
    }

    bool GameApp::init()
    {
        spdlog::info("GameApp init ...");
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        {
            spdlog::error("SDL_Init failed: {}", SDL_GetError());
            return false;
        }
        _window = SDL_CreateWindow("Mario_DiePI", 1280, 720, SDL_WINDOW_RESIZABLE);
        if (_window == nullptr)
        {
            spdlog::error("SDL_CreateWindow failed: {}", SDL_GetError());
            return false;
        }
        _sdl_renderer = SDL_CreateRenderer(_window, nullptr);
        if (_sdl_renderer == nullptr)
        {
            spdlog::error("SDL_CreateRenderer failed: {}", SDL_GetError());
            return false;
        }
        _is_running = true;
        return true;
    }

    void GameApp::handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                _is_running = false;
            }
        }
    }

    void GameApp::update(float dt)
    {
    }

    void GameApp::render()
    {
    }

    void GameApp::close()
    {
        spdlog::info("GameApp close ...");
        if (_sdl_renderer != nullptr)
        {
            SDL_DestroyRenderer(_sdl_renderer);
            _sdl_renderer = nullptr;
        }
        if (_window != nullptr)
        {
            SDL_DestroyWindow(_window);
            _window = nullptr;
        }

        SDL_Quit();
        _is_running = false;
    }

}