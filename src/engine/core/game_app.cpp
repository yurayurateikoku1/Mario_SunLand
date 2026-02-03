#include "game_app.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include "time.h"
#include "context.h"
#include "../resource/resource_manager.h"
#include "../audio/audio_player.h"
#include "../render/camera.h"
#include "../render/render.h"
#include "../input/input_manager.h"
#include "../object/game_object.h"
#include "../component/transform_component.h"
#include "../component/sprite_component.h"
#include "../physics/physics_engine.h"
#include "../component/physics_component.h"
#include "../scene/scene_manager.h"
#include "../../game/scene/game_scene.h"
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
            _scene_manager->handleInput();
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

    bool GameApp::initAudioPlayer()
    {
        try
        {
            _audio_player = std::make_unique<engine::audio::AudioPlayer>(_resource_manager.get());
        }
        catch (const std::exception &e)
        {
            spdlog::error("AudioPlayer init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
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

    bool GameApp::initPhysicsEngine()
    {
        try
        {
            _physics_engine = std::make_unique<engine::physics::PhysicsEngine>();
        }
        catch (const std::exception &e)
        {
            spdlog::error("PhysicsEngine init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        return true;
    }

    bool GameApp::initContext()
    {
        try
        {
            _context = std::make_unique<engine::core::Context>(*_input_manager, *_renderer, *_resource_manager, *_camera, *_physics_engine, *_audio_player);
        }
        catch (const std::exception &e)
        {
            spdlog::error("Context init failed: {},{},{}", e.what(), __FILE__, __LINE__);
            return false;
        }
        return true;
    }

    bool GameApp::initSceneManager()
    {
        try
        {
            _scene_manager = std::make_unique<engine::scene::SceneManager>(*_context);
        }
        catch (const std::exception &e)
        {
            spdlog::error("SceneManager init failed: {},{},{}", e.what(), __FILE__, __LINE__);
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
        if (!initAudioPlayer())
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
        if (!initPhysicsEngine())
        {
            return false;
        }
        if (!initContext())
        {
            return false;
        }
        if (!initSceneManager())
        {
            /* code */
            return false;
        }

        auto scene = std::make_unique<game::scene::GameScene>(*_context, *_scene_manager);
        _scene_manager->requestPushScene(std::move(scene));

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
        _scene_manager->update(dt);
    }

    void GameApp::render()
    {
        _renderer->clearScreen();
        _scene_manager->render();
        _renderer->present();
    }

    void GameApp::close()
    {
        spdlog::info("GameApp close ...");
        _scene_manager->close();
        _resource_manager.reset();
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