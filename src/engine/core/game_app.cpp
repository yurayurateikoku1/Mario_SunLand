#include "game_app.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include "time.h"
#include "../resource/resource_manager.h"
#include "../render/camera.h"
#include "../render/render.h"
namespace engine::core
{
    GameApp::GameApp()
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

    bool GameApp::initSDL()
    {
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
        SDL_SetRenderLogicalPresentation(_sdl_renderer, 640, 360, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        return true;
    }

    bool GameApp::initTime()
    {
        try
        {
            _time = std::make_unique<Time>();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Time init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        return true;
    }

    bool GameApp::initResourceManager()
    {
        try
        {
            _resource_manager = std::make_unique<engine::resource::ResourceManager>(_sdl_renderer);
        }
        catch (const std::exception &e)
        {
            spdlog::error("ResourceManager init failed: {},{},{}", e.what(), __FILE__, __LINE__);
        }
        return true;
    }

    bool GameApp::initRenderer()
    {
        try
        {
            _renderer = std::make_unique<engine::render::Renderer>(_sdl_renderer, _resource_manager.get());
        }
        catch (const std::exception &e)
        {
            spdlog::error("Renderer init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }

        return true;
    }

    bool GameApp::initCamera()
    {
        try
        {
            _camera = std::make_unique<engine::render::Camera>(glm::vec2(640, 360));
        }
        catch (const std::exception &e)
        {
            spdlog::error("Camera init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }

        return true;
    }

    bool GameApp::init()
    {
        spdlog::info("GameApp init ...");
        if (!initSDL())
        {
            return false;
        }
        if (!initTime())
        {
            return false;
        }
        if (!initResourceManager())
        {
            return false;
        }
        if (!initRenderer())
        {
            return false;
        }
        if (!initCamera())
        {
            return false;
        }

        _is_running = true;
        spdlog::info("GameApp init success");
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

    void GameApp::update([[maybe_unused]] float dt)
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