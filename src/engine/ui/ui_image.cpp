#include "ui_image.h"
#include "../render/render.h"
#include "../render/sprite.h"
#include "../core/context.h"
#include <spdlog/spdlog.h>

engine::ui::UIImage::UIImage(const std::string &texture_id, const glm::vec2 &position, const glm::vec2 &size, const std::optional<SDL_Rect> &source_rect, bool is_flipped)
    : UIElement(position, size), _sprite(texture_id, source_rect, is_flipped)
{
    if (texture_id.empty())
    {
        spdlog::error("texture_id is empty");
    }
    spdlog::info("UIImage created: {}", texture_id);
}

void engine::ui::UIImage::render(engine::core::Context &context)
{
    if (!_visible || _sprite.getTextureId().empty())
    {
        return;
    }

    auto position = getScreenPosition();
    if (_size.x == 0.0f && _size.y == 0.0f)
    {
        context.getRender().drawUISprite(_sprite, position);
    }
    else
    {
        context.getRender().drawUISprite(_sprite, position, _size);
    }
    UIElement::render(context);
}
