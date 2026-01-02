#include "config.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
engine::core::Config::Config(const std::string &file_path)
{
    loadFromFile(file_path);
}

bool engine::core::Config::loadFromFile(const std::string &file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        spdlog::warn("Config file not found: {}", file_path);

        if (!save2File(file_path))
        {
            spdlog::error("Failed to save config file: {}", file_path);
            return false;
        }
        return false;
    }
    try
    {
        nlohmann::json j;
        file >> j;
        fromJson(j);
        spdlog::info("Config file loaded: {}", file_path);
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to save config file: {},{},{},{}", file_path, e.what(), __FILE__, __LINE__);
    }
    return false;
}

bool engine::core::Config::save2File(const std::string &file_path)
{
    std::ofstream file(file_path);
    if (!file.is_open())
    {
        spdlog::error("Failed to save config file: {}", file_path);
        return false;
    }
    try
    {
        nlohmann::ordered_json j = toJson();
        file << j.dump(4);
        spdlog::info("Saved config file: {}", file_path);
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to save config file: {},{},{},{}", file_path, e.what(), __FILE__, __LINE__);
    }
    return false;
}

void engine::core::Config::fromJson(const nlohmann::json &j)
{
    if (j.contains("window"))
    {
        const auto &window_config = j["window"];
        _window_title = window_config.value("title", _window_title);
        _window_width = window_config.value("width", _window_width);
        _window_height = window_config.value("height", _window_height);
        _window_resizable = window_config.value("resizable", _window_resizable);
    }
    if (j.contains("graphics"))
    {
        const auto &graphics = j["graphics"];
        _vsync_enabled = graphics.value("vsync", _vsync_enabled);
    }
    if (j.contains("performance"))
    {
        /* code */
        const auto &perf_config = j["performance"];
        _target_fps = perf_config.value("target_fps", _target_fps);
        if (_target_fps < 0)
        {
            spdlog::warn("Target FPS must be greater than 0");
            _target_fps = 0;
        }
    }
    if (j.contains("audio"))
    {
        const auto &audio_config = j["audio"];
        _music_volume = audio_config.value("music_volume", _music_volume);
        _sound_volume = audio_config.value("sound_volume", _sound_volume);
    }
    if (j.contains("input_mappings") && j["input_mappings"].is_object())
    {
        const auto mappings_json = j["input_mappings"];
        try
        {
            auto input_mappings = mappings_json.get<std::unordered_map<std::string, std::vector<std::string>>>();
            _input_mappings = std::move(input_mappings);
            spdlog::info("Input mappings loaded");
        }
        catch (const std::exception &e)
        {
            spdlog::warn("Use default settings,Failed to load input mappings: {},", e.what());
        }
    }
    else
    {
        spdlog::warn("Use default settings,Failed to load input mappings");
    }
}

nlohmann::ordered_json engine::core::Config::toJson() const
{

    return nlohmann::ordered_json{
        {
            "window",
            {
                {"title", _window_title},
                {"width", _window_width},
                {"height", _window_height},
                {"resizable", _window_resizable},
            },
        },
        {
            "graphics",
            {
                {"vsync", _vsync_enabled},
            },
        },
        {
            "performance",
            {
                {"target_fps", _target_fps},
            },
        },
        {
            "audio",
            {
                {"music_volume", _music_volume},
                {"sound_volume", _sound_volume},
            },
        },
        {"input_mappings",
         _input_mappings}};
}
