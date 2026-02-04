#pragma once
#include "ui_interactive.h"
#include "functional"
#include <utility>

namespace engine::ui
{
    class UIButton : public UIInteractive
    {
    private:
        std::function<void()> _callback;

    public:
        UIButton(engine::core::Context &context, const std::string &norml_sprite_id, const std::string &hover_sprite_id,
                 const std::string &pressed_sprite_id, const glm::vec2 &position = {0.0f, 0.0f}, const glm::vec2 &size = {0.0f, 0.0f},
                 std::function<void()> callback = nullptr);
        ~UIButton() override = default;
        void clicked() override;

        void setCallback(std::function<void()> callback) { _callback = std::move(callback); }
        std::function<void()> getCallback() const { return _callback; }
    };
}