#include "ui_hover_state.h"
#include "ui_normal_state.h"
#include "ui_press_state.h"
#include "../ui_interactive.h"
#include "../../input/input_manager.h"
#include "../../core/context.h"
#include <spdlog/spdlog.h>

namespace engine::ui::state
{

    void UIHoverState::enter()
    {
        _owner->setSprite("hover");
    }

    std::unique_ptr<UIState> UIHoverState::handleInput(engine::core::Context &context)
    {
        auto &input_manager = context.getInputManager();
        auto mouse_pos = input_manager.getLogicalMousePosition();
        if (!_owner->isPointInside(mouse_pos))
        { // 如果鼠标不在UI元素内，则返回正常状态
            return std::make_unique<UINormalState>(_owner);
        }
        if (input_manager.isActionPressed("MouseLeftClick"))
        { // 如果鼠标按下，则返回按下状态
            return std::make_unique<UIPressState>(_owner);
        }
        return nullptr;
    }

} // namespace engine::ui::state