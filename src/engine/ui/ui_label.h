#pragma once
#include "ui_element.h"
#include "../utils/math.h"
#include "../render/text_renderer.h"
#include <string>

namespace engine::ui
{
    class UILabel final : public UIElement
    {
    private:
        engine::render::TextRenderer &_text_renderer;
        std::string _text;
        std::string _font_id;
        int _font_size;
        engine::utils::FColor _text_fcolor = {1.0f, 1.0f, 1.0f, 1.0f};

    public:
        UILabel(engine::render::TextRenderer &text_renderer,
                const std::string &text,
                const std::string &font_id,
                int font_size = 16,
                engine::utils::FColor text_color = {1.0f, 1.0f, 1.0f, 1.0f},
                glm::vec2 position = {0.0f, 0.0f});

        void render(engine::core::Context &context) override;

        std::string getText() const { return _text; }
        std::string getFontId() const { return _font_id; }
        int getFontSize() const { return _font_size; }
        const engine::utils::FColor &getTextFColor() const { return _text_fcolor; }

        void setText(const std::string &text);      ///< @brief 设置文本内容, 同时更新尺寸
        void setFontId(const std::string &font_id); ///< @brief 设置字体ID, 同时更新尺寸
        void setFontSize(int font_size);            ///< @brief 设置字体大小, 同时更新尺寸
        void setTextFColor(engine::utils::FColor text_fcolor);
    };
}