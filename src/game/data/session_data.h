#pragma once
#include <string>
#include <memory>
#include <nlohmann/json.hpp>

namespace game::data
{
    class SessionData final
    {
    private:
        int _current_health = 3;
        int _max_health = 3;
        int _current_score = 0;
        int _high_score = 0;
        bool _is_win = false;
        int _level_health = 3;
        int _level_score = 0;
        std::string _map_path = "assets/maps/level1.tmj";

    public:
        SessionData() = default;
        ~SessionData() = default;
        SessionData(const SessionData &) = delete;
        SessionData(SessionData &&) = delete;
        SessionData &operator=(const SessionData &) = delete;
        SessionData &operator=(SessionData &&) = delete;
        int getCurrentHealth() const { return _current_health; }
        int getMaxHealth() const { return _max_health; }
        int getCurrentScore() const { return _current_score; }
        int getHighScore() const { return _high_score; }
        int getLevelHealth() const { return _level_health; }
        int getLevelScore() const { return _level_score; }
        const std::string &getMapPath() const { return _map_path; }
        bool getIsWin() const { return _is_win; }

        void addScore(int score);
        void setCurrentHealth(int health);
        void setMaxHealth(int health);
        void setHighScore(int score);
        void setLevelHealth(int health);
        void setLevelScore(int score);
        void setMapPath(const std::string &path);
        void setIsWin(bool is_win) { _is_win = is_win; }
        void reset();
        void setNextLevel(const std::string &map_path);
        bool saveToFile(const std::string &file_path) const;
        bool loadFromFile(const std::string &file_path);

        void syncHighScore(const std::string &file_path);
    };
}