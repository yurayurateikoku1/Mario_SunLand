#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL_render.h>
#include <glm/vec2.hpp>

namespace engine::core
{
    class Config;
}

namespace engine::input
{
    enum class ActionState
    {
        INACTIVE,
        PRESSED,
        HELD,
        RELEASED
    };

    class InputManager final
    {
    private:
        /* data */
        SDL_Renderer *_sdl_renderer{nullptr};

        std::unordered_map<std::string, std::vector<std::string>> _action2keyname_map;
        std::unordered_map<SDL_Scancode, std::vector<std::string>> _scancode2action_map;
        std::unordered_map<Uint8, std::vector<std::string>> _mousebutton2action_map;
        std::unordered_map<std::string, ActionState> _action_states{};

        bool _should_quit{false};
        glm::vec2 _mouse_position{0.0f, 0.0f};

    public:
        InputManager(SDL_Renderer *sdl_renderer, const engine::core::Config *config);

        void update();

        /// @brief 动作当前是否触发,持续按压
        /// @param action_name
        /// @return
        bool isActionDown(const std::string &action_name) const;

        /// @brief 动作当前是否在本帧刚刚按下
        /// @param action_name
        /// @return
        bool isActionPressed(const std::string &action_name) const;

        /// @brief 动作当前是否在本帧刚刚释放
        /// @param action_name
        /// @return
        bool isActionReleased(const std::string &action_name) const;

        bool getShouldQuit() const { return _should_quit; }
        void setShouldQuit(bool quit) { _should_quit = quit; }

        glm::vec2 getMousePosition() const { return _mouse_position; }
        glm::vec2 getLogicalMousePosition() const;

    private:
        /// @brief 处理事件，将按键状态转换为动作状态
        /// @param event
        void processEvent(const SDL_Event &event);
        void initMappings(const engine::core::Config *config);
        void updateActionStates(const std::string &action_name, bool is_input_active, bool is_reapt_event);
        SDL_Scancode scancodeFromString(const std::string &key_name);
        Uint8 mouseButtonFromString(const std::string &button_name);
    };

}
