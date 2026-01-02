#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json_fwd.hpp>

namespace engine::core
{
    class Config final
    {
    public:
        /* data */
        std::string _window_title = "Mario_DiePI";
        int _window_width = 1280;
        int _window_height = 720;
        bool _window_resizable = true;

        bool _vsync_enabled = true;
        int _target_fps = 60;
        float _music_volume = 0.5f;
        float _sound_volume = 0.5f;

        /// @brief 存储动作名称到SDL Scancode名称的列表映射
        std::unordered_map<std::string, std::vector<std::string>> _input_mappings =
            {
                {"move_left", {"A", "Left"}},
                {"move_right", {"D", "Right"}},
                {"move_up", {"W", "Up"}},
                {"move_down", {"S", "Down"}},
                {"jump", {"J", "Space"}},
                {"attack", {"K", "MouseLeft"}},
                {"pause", {"P", "Escape"}}};
        explicit Config(const std::string &file_path);
        Config(const Config &) = delete;
        Config &operator=(const Config &) = delete;
        Config(Config &&) = delete;
        Config &operator=(Config &&) = delete;

        bool loadFromFile(const std::string &file_path);
        [[nodiscard]] bool save2File(const std::string &file_path);

    private:
        /// @brief
        /// @param j
        void fromJson(const nlohmann::json &j);
        nlohmann::ordered_json toJson() const;
    };

}