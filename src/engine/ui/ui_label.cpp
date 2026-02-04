#include "ui_label.h"
#include "../core/context.h"
#include "../render/text_renderer.h"
#include <spdlog/spdlog.h>
engine::ui::UILabel::UILabel(engine::render::TextRenderer &text_renderer, const std::string &text, const std::string &font_id, int font_size, engine::utils::FColor text_color, glm::vec2 position)
    : UIElement(std::move(position)),
      _text_renderer(text_renderer),
      _text(text),
      _font_id(font_id),
      _font_size(font_size),
      _text_fcolor(std::move(text_color))
{
    _size = _text_renderer.getTextSize(_text, _font_id, _font_size);
    spdlog::trace("UILabel size: {} x {}", _size.x, _size.y);
}

void engine::ui::UILabel::render(engine::core::Context &context)
{
    if (!_visible || _text.empty())
        return;

    _text_renderer.drawUIText(_text, _font_id, _font_size, getScreenPosition(), _text_fcolor);

    UIElement::render(context);
}

void engine::ui::UILabel::setText(const std::string &text)
{
    _text = text;
    _size = _text_renderer.getTextSize(_text, _font_id, _font_size);
}

void engine::ui::UILabel::setFontId(const std::string &font_id)
{
    _font_id = font_id;
    _size = _text_renderer.getTextSize(_text, _font_id, _font_size);
}

void engine::ui::UILabel::setFontSize(int font_size)
{
    _font_size = font_size;
    _size = _text_renderer.getTextSize(_text, _font_id, _font_size);
}

void engine::ui::UILabel::setTextFColor(engine::utils::FColor text_fcolor)
{
    _text_fcolor = std::move(text_fcolor);
}
