#include "game_app.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include "time.h"
#include "../resource/resource_manager.h"
#include "../render/camera.h"
#include "../render/render.h"
#include "../input/input_manager.h"
#include "config.h"
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
        while (_is_running)
        {
            _time->update();
            float dt = _time->getDeltaTime();
            _input_manager->update();
            handleEvents();
            update(dt);
            render();
        }
        close();
    }

    bool GameApp::initConfig()
    {
        try
        {
            _config = std::make_unique<engine::core::Config>("assets/config.json");
        }
        catch (const std::exception &e)
        {
            spdlog::error("Config init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        spdlog::info("Config init success");
        return true;
    }

    bool GameApp::initSDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        {
            spdlog::error("SDL_Init failed: {}", SDL_GetError());
            return false;
        }
        _window = SDL_CreateWindow(_config->_window_title.c_str(), _config->_window_width, _config->_window_height, SDL_WINDOW_RESIZABLE);
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

        int vsync_code = _config->_vsync_enabled ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED;
        SDL_SetRenderVSync(_sdl_renderer, vsync_code);

        SDL_SetRenderLogicalPresentation(_sdl_renderer, _config->_window_width / 2, _config->_window_height / 2, SDL_LOGICAL_PRESENTATION_LETTERBOX);
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
        _time->setTargetFps(_config->_target_fps);
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
            _camera = std::make_unique<engine::render::Camera>(glm::vec2(_config->_window_width / 2, _config->_window_height / 2));
        }
        catch (const std::exception &e)
        {
            spdlog::error("Camera init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }

        return true;
    }

    bool GameApp::initInputManager()
    {
        try
        {
            _input_manager = std::make_unique<engine::input::InputManager>(_sdl_renderer, _config.get());
        }
        catch (const std::exception &e)
        {
            spdlog::error("InputManager init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        return true;
    }

    bool GameApp::init()
    {
        spdlog::info("GameApp init ...");
        if (!initConfig())
        {
            return false;
        }
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
        if (!initInputManager())
        {
            return false;
        }

        _is_running = true;
        spdlog::info("GameApp init success");
        return true;
    }

    void GameApp::handleEvents()
    {
        if (_input_manager->getShouldQuit())
        {
            /* code */
            _is_running = false;
            return;
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