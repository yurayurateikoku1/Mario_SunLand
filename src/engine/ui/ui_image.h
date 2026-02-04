#pragma once
#include "ui_element.h"
#include "../render/sprite.h"
#include <string>
#include <string_view>
#include <optional>
#include <SDL3/SDL_rect.h>

namespace engine::ui
{
    class UIImage final : public UIElement
    {
    protected:
        engine::render::Sprite _sprite;

    public:
        UIImage(const std::string &texture_id, const glm::vec2 &position = {0.0f, 0.0f}, const glm::vec2 &size = {0.0f, 0.0f},
                const std::optional<SDL_Rect> &source_rect = std::nullopt, bool is_flipped = false);
        void render(engine::core::Context &context) override;

        const engine::render::Sprite &getSprite() const { return _sprite; }
        void setSprite(engine::render::Sprite sprite) { _sprite = std::move(sprite); }

        std::string getTextureId() const { return _sprite.getTextureId(); }
        void setTextureId(const std::string &texture_id) { _sprite.setTextureId(texture_id); }

        const std::optional<SDL_Rect> &getSourceRect() const { return _sprite.getSourceRect(); }
        void setSourceRect(std::optional<SDL_Rect> source_rect) { _sprite.setSourceRect(std::move(source_rect)); }

        bool isFlipped() const { return _sprite.isFlipped(); }
        void setFlipped(bool flipped) { _sprite.setFlipped(flipped); }
    };
}