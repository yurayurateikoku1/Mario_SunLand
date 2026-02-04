#include "session_data.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <glm/common.hpp>
void game::data::SessionData::addScore(int score)
{
    _current_score += score;
    setHighScore(glm::max(_current_score, _high_score));
}

void game::data::SessionData::setCurrentHealth(int health)
{
    _current_health = glm::clamp(health, 0, _max_health);
}

void game::data::SessionData::setMaxHealth(int health)
{
    if (health > 0)
    {
        /* code */
        _max_health = health;
        setCurrentHealth(_current_health);
    }
    else
    {
        spdlog::warn("Health must be greater than 0");
    }
}

void game::data::SessionData::setHighScore(int score)
{
    _high_score = score;
}

void game::data::SessionData::setLevelHealth(int health)
{
    _level_health = health;
}

void game::data::SessionData::setLevelScore(int score)
{
    _level_score = score;
}

void game::data::SessionData::setMapPath(const std::string &path)
{
    _map_path = path;
}

void game::data::SessionData::reset()
{
    _current_health = _max_health;
    _current_score = 0;
    _level_health = 3;
    _level_score = 0;
    _map_path = "assets/maps/level1.tmj";
    spdlog::info("Session data reset");
}

void game::data::SessionData::setNextLevel(const std::string &map_path)
{
    _map_path = map_path;
    _level_health = _current_health;
    _level_score = _current_score;
}

bool game::data::SessionData::saveToFile(const std::string &file_path) const
{
    nlohmann::json j;
    try
    {
        j["level_score"] = _level_score;
        j["level_health"] = _level_health;
        j["map_health"] = _max_health;
        j["high_score"] = _high_score;
        j["map_path"] = _map_path;

        std::ofstream file(file_path);
        if (!file.is_open())
        {
            return false;
        }

        file << j.dump(4);
        file.close();
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to save config file: {},{},{},{}", file_path, e.what(), __FILE__, __LINE__);
        return false;
    }
}

bool game::data::SessionData::loadFromFile(const std::string &file_path)
{
    try
    {
        std::ifstream ifs(file_path);
        if (!ifs.is_open())
        {
            return false;
        }

        nlohmann::json j;
        ifs >> j;
        ifs.close();
        _current_score = _level_score = j.value("level_score", 0);
        _current_health = _level_health = j.value("level_health", 3);
        _max_health = j.value("map_health", 3);
        _high_score = glm::max(_high_score, j.value("high_score", 0));
        _map_path = j.value("map_path", "assets/maps/level1.tmj");
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to load config file: {},{},{},{}", file_path, e.what(), __FILE__, __LINE__);
        reset();
        return false;
    }
}

void game::data::SessionData::syncHighScore(const std::string &file_path)
{
    try
    {
        // 打开文件进行读取
        auto path = std::filesystem::path(file_path);
        std::fstream fs(path);
        if (!fs.is_open())
        {
            return;
        }

        // 从文件解析 JSON 数据
        nlohmann::json j;
        fs >> j;
        auto high_score_in_file = j.value("high_score", 0);

        // 根据文件中的最高分和当前最高分来决定处理方式
        if (high_score_in_file < _high_score)
        { // 文件中的最高分 低于 当前最高分
            j["high_score"] = _high_score;
            fs.seekp(0);     // 文件指针回到文件开头
            fs << j.dump(4); // 将JSON对象写入文件
        }
        else if (high_score_in_file > _high_score)
        { // 文件中的最高分 高于 当前最高分
            _high_score = high_score_in_file;
        }
        else
        {
        }
        fs.close();
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to sync high score: {},{},{},{}", file_path, e.what(), __FILE__, __LINE__);
    }
}
