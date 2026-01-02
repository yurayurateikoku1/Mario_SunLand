#include "input_manager.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>
#include "../core/config.h"
#include <glm/vec2.hpp>
engine::input::InputManager::InputManager(SDL_Renderer *sdl_renderer, const engine::core::Config *config)
{
    if (!sdl_renderer)
    {
        spdlog::error("SDL_Renderer is null");
        throw std::runtime_error("SDL_Renderer is null");
    }
    initMappings(config);
    float x, y;
    SDL_GetMouseState(&x, &y);
    _mouse_position = glm::vec2(x, y);
}

void engine::input::InputManager::update()
{
    for (auto &[action_name, state] : _action_states)
    {
        if (state == ActionState::PRESSED)
        {
            state = ActionState::HELD;
        }
        else if (state == ActionState::RELEASED)
        {
            state = ActionState::INACTIVE;
        }
    }
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        processEvent(event);
    }
}

bool engine::input::InputManager::isActionDown(const std::string &action_name) const
{
    if (auto it = _action_states.find(action_name); it != _action_states.end())
    {
        return it->second == ActionState::HELD || it->second == ActionState::PRESSED;
    }

    return false;
}

bool engine::input::InputManager::isActionPressed(const std::string &action_name) const
{
    if (auto it = _action_states.find(action_name); it != _action_states.end())
    {
        return it->second == ActionState::PRESSED;
    }
    return false;
}

bool engine::input::InputManager::isActionReleased(const std::string &action_name) const
{
    if (auto it = _action_states.find(action_name); it != _action_states.end())
    {
        return it->second == ActionState::RELEASED;
    }
    return false;
}

glm::vec2 engine::input::InputManager::getLogicalMousePosition() const
{
    glm::vec2 logical_position;
    SDL_RenderCoordinatesFromWindow(_sdl_renderer, _mouse_position.x, _mouse_position.y, &logical_position.x, &logical_position.y);
    return logical_position;
}

void engine::input::InputManager::processEvent(const SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
    {
        SDL_Scancode scancode = event.key.scancode;
        bool is_down = event.key.down;
        bool is_repeat = event.key.repeat;
        auto it = _scancode2action_map.find(scancode);
        if (it != _scancode2action_map.end())
        {
            const std::vector<std::string> &associated_actions = it->second;
            for (const std::string &action_name : associated_actions)
            {
                updateActionStates(action_name, is_down, is_repeat);
            }
        }
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
        Uint8 mouse_button = event.button.button;
        bool is_down_mouse = event.button.down;
        auto it_mouse = _mousebutton2action_map.find(mouse_button);
        if (it_mouse != _mousebutton2action_map.end())
        {
            const std::vector<std::string> &associated_actions = it_mouse->second;
            for (const std::string &action_name : associated_actions)
            {
                updateActionStates(action_name, is_down_mouse, false);
            }
        }
        break;
    }
    case SDL_EVENT_MOUSE_MOTION:
    {
        _mouse_position = {event.motion.x, event.motion.y};
        break;
    }
    case SDL_EVENT_QUIT:
    {
        _should_quit = true;
        break;
    }
    default:
        break;
    }
}

void engine::input::InputManager::initMappings(const engine::core::Config *config)
{
    if (!config)
    {
        spdlog::error("Config is null");
        throw std::runtime_error("Config is null");
    }
    _action2keyname_map = config->_input_mappings;
    _scancode2action_map.clear();
    _mousebutton2action_map.clear();
    _action_states.clear();

    if (_action2keyname_map.find("MouseLeftClick") == _action2keyname_map.end())
    {
        _action2keyname_map["MouseLeftClick"] = {"MouseLeft"};
    }
    if (_action2keyname_map.find("MouseRightClick") == _action2keyname_map.end())
    {
        _action2keyname_map["MouseRightClick"] = {"MouseRight"};
    }

    for (const auto &[action_name, key_names] : _action2keyname_map)
    {
        _action_states[action_name] = ActionState::INACTIVE;
        for (const std::string &key_name : key_names)
        {
            /* code */
            SDL_Scancode scancode = scancodeFromString(key_name);
            Uint8 mouse_button = mouseButtonFromString(key_name);

            if (scancode != SDL_SCANCODE_UNKNOWN)
            {
                /* code */
                _scancode2action_map[scancode].push_back(action_name);
            }
            else if (mouse_button != 0)
            {
                _mousebutton2action_map[mouse_button].push_back(action_name);
            }
            else
            {
                spdlog::warn("Unknown key name: {}", key_name);
            }
        }
    }
}

void engine::input::InputManager::updateActionStates(const std::string &action_name, bool is_input_active, bool is_reapt_event)
{
    auto it = _action_states.find(action_name);
    if (it == _action_states.end())
    {
        return;
    }
    if (is_input_active)
    {
        if (is_reapt_event)
        {
            it->second = ActionState::HELD;
        }
        else
        {
            it->second = ActionState::PRESSED;
        }
    }
    else
    {
        it->second = ActionState::RELEASED;
    }
}

SDL_Scancode engine::input::InputManager::scancodeFromString(const std::string &key_name)
{
    return SDL_GetScancodeFromName(key_name.c_str());
}

Uint8 engine::input::InputManager::mouseButtonFromString(const std::string &button_name)
{
    if (button_name == "MouseLeft")
    {
        return SDL_BUTTON_LEFT;
    }
    else if (button_name == "MouseMiddle")
    {
        return SDL_BUTTON_MIDDLE;
    }
    else if (button_name == "MouseRight")
    {
        return SDL_BUTTON_RIGHT;
    }
    else if (button_name == "MouseX1")
    {
        return SDL_BUTTON_X1;
    }
    else if (button_name == "MouseX2")
    {
        return SDL_BUTTON_X2;
    }

    return 0;
}
