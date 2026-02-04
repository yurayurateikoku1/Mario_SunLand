#include "engine/core/game_app.h"
#include <SDL3/SDL_main.h>
#include <spdlog/spdlog.h>
#include "engine/scene/scene_manager.h"
#include "game/scene/splash_scene.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
void setupInitialScene(engine::scene::SceneManager &sceneManager)
{
    auto splashScene = std::make_unique<game::scene::SplashScene>(sceneManager.getContext(), sceneManager);
    sceneManager.requestPushScene(std::move(splashScene));
}
int main(int, char **)
{

    try
    {
        auto file_logger = spdlog::basic_logger_mt("basic_logger", "log.txt");
        spdlog::set_default_logger(file_logger);
        spdlog::set_level(spdlog::level::trace);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        spdlog::flush_on(spdlog::level::info);
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        // 如果文件日志初始化失败，输出错误到控制台
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    engine::core::GameApp app;
    app.registerSceneSutep(setupInitialScene);
    app.run();

    spdlog::shutdown();
    return 0;
}