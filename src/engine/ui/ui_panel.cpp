#include "ui_panel.h"
#include "../core/context.h"
#include "../render/render.h"
#include <SDL3/SDL_rect.h>
#include <spdlog/spdlog.h>
engine::ui::UIPanel::UIPanel(const glm::vec2 &position, const glm::vec2 &size, std::optional<engine::utils::FColor> background_color)
    : UIElement(position, size), _background_color(background_color)
{
    spdlog::info("UIPanel created");
}

void engine::ui::UIPanel::render(engine::core::Context &context)
{
    if (!_visible)
    {
        return;
    }
    if (_background_color)
    {
        context.getRender().drawUIFillRect(getBounds(), _background_color.value());
    }
    UIElement::render(context);
}
