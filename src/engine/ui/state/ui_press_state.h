#pragma once
#include "ui_state.h"

namespace engine::ui::state
{
    class UIPressState final : public UIState
    {
        friend class engine::ui::UIInteractive;

    public:
        UIPressState(engine::ui::UIInteractive *owner) : UIState(owner) {}
        ~UIPressState() override = default;

    private:
        void enter() override;
        std::unique_ptr<UIState> handleInput(engine::core::Context &) override;
    };
}