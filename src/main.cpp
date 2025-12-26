#include "engine/core/game_app.h"
#include <SDL3/SDL_main.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
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
    app.run();

    spdlog::shutdown();
    return 0;
}