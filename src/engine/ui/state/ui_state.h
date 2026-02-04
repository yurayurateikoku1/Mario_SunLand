#pragma once
#include <memory>
namespace engine::core
{
    class Context;
}

namespace engine::ui
{
    class UIInteractive;
}

namespace engine::ui::state
{
    class UIState
    {
        friend class engine::ui::UIInteractive;

    protected:
        engine::ui::UIInteractive *_owner = nullptr;

    public:
        UIState(engine::ui::UIInteractive *owner) : _owner(owner) {}
        virtual ~UIState() = default;
        UIState(const UIState &) = delete;
        UIState(UIState &&) = delete;
        UIState &operator=(const UIState &) = delete;
        UIState &operator=(UIState &&) = delete;

    protected:
        virtual void enter() {}
        virtual std::unique_ptr<UIState> handleInput(engine::core::Context &) = 0;
    };
}