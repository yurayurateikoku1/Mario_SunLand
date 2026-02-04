#include "ui_button.h"
#include "state/ui_normal_state.h"
#include <spdlog/spdlog.h>
engine::ui::UIButton::UIButton(engine::core::Context &context, const std::string &norml_sprite_id, const std::string &hover_sprite_id, const std::string &pressed_sprite_id, const glm::vec2 &position, const glm::vec2 &size, std::function<void()> callback)
    : UIInteractive(context, position, size), _callback(std::move(callback))
{
    addSprite("normal", std::make_unique<engine::render::Sprite>(norml_sprite_id));
    addSprite("hover", std::make_unique<engine::render::Sprite>(hover_sprite_id));
    addSprite("pressed", std::make_unique<engine::render::Sprite>(pressed_sprite_id));
    setState(std::make_unique<engine::ui::state::UINormalState>(this));
    addSound("hover", "assets/audio/button_hover.wav");
    addSound("pressed", "assets/audio/button_click.wav");
    spdlog::info("UIButton created");
}

void engine::ui::UIButton::clicked()
{
    if (_callback)
    {
        _callback();
    }
}
