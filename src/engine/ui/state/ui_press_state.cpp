#include "ui_press_state.h"
#include "ui_normal_state.h"
#include "ui_hover_state.h"
#include "../ui_interactive.h"
#include "../../input/input_manager.h"
#include "../../core/context.h"
#include <spdlog/spdlog.h>

namespace engine::ui::state
{

    void UIPressState::enter()
    {
        _owner->setSprite("pressed");
        _owner->playSound("pressed");
        spdlog::debug("切换到按下状态");
    }

    std::unique_ptr<UIState> UIPressState::handleInput(engine::core::Context &context)
    {
        auto &input_manager = context.getInputManager();
        auto mouse_pos = input_manager.getLogicalMousePosition();
        if (input_manager.isActionReleased("MouseLeftClick"))
        {
            if (!_owner->isPointInside(mouse_pos))
            { // 松开鼠标时，如果不在UI元素内，则切换到正常状态
                return std::make_unique<engine::ui::state::UINormalState>(_owner);
            }
            else
            { // 松开鼠标时，如果还在UI元素内，则触发点击事件
                _owner->clicked();
                return std::make_unique<engine::ui::state::UIHoverState>(_owner);
            }
        }

        return nullptr;
    }

} // namespace engine::ui::state