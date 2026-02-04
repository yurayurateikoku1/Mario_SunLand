#include "game_state.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::core
{

    GameState::GameState(SDL_Window *window, SDL_Renderer *renderer, State initial_state)
        : _window(window), _renderer(renderer), _current_state(initial_state)
    {
        if (_window == nullptr || _renderer == nullptr)
        {
            throw std::runtime_error("window or renderer is nullptr");
        }
    }

    void GameState::setState(State new_state)
    {
        if (_current_state != new_state)
        {
            _current_state = std::move(new_state);
        }
        else
        {
            spdlog::warn("new_state is the same as current_state");
        }
    }

    glm::vec2 GameState::getWindowSize() const
    {
        int width, height;
        // SDL3获取窗口大小的方法
        SDL_GetWindowSize(_window, &width, &height);
        return glm::vec2(width, height);
    }

    void GameState::setWindowSize(const glm::vec2 &window_size)
    {
        SDL_SetWindowSize(_window, static_cast<int>(window_size.x), static_cast<int>(window_size.y));
    }

    glm::vec2 GameState::getLogicalSize() const
    {
        int width, height;
        // SDL3获取逻辑分辨率的方法
        SDL_GetRenderLogicalPresentation(_renderer, &width, &height, NULL);
        return glm::vec2(width, height);
    }

    void GameState::setLogicalSize(const glm::vec2 &logical_size)
    {
        SDL_SetRenderLogicalPresentation(_renderer,
                                         static_cast<int>(logical_size.x),
                                         static_cast<int>(logical_size.y),
                                         SDL_LOGICAL_PRESENTATION_LETTERBOX);
        spdlog::trace("Set logical size: {}x{}", logical_size.x, logical_size.y);
    }

} // namespace engine::core