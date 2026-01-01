#pragma once
#include <SDL3/SDL_rect.h>
#include <optional>
#include <string>

namespace engine::render
{
    class Sprite
    {
    private:
        /// @brief 纹理ID
        std::string _texture_id;
        /// @brief 源矩形
        std::optional<SDL_Rect> _source_rect;
        /// @brief 是否翻转
        bool _is_flipped = false;

    public:
        /// @brief 构造函数
        /// @param texture_id
        /// @param source_rect
        /// @param is_flipped
        Sprite(const std::string &texture_id, std::optional<SDL_Rect> source_rect = std::nullopt, bool is_flipped = false)
            : _texture_id(texture_id), _source_rect(source_rect), _is_flipped(is_flipped) {}
        const std::string &getTextureId() const { return _texture_id; }
        const std::optional<SDL_Rect> &getSourceRect() const { return _source_rect; }
        bool isFlipped() const { return _is_flipped; }
        void setTextureId(const std::string &texture_id) { _texture_id = texture_id; }
        void setSourceRect(std::optional<SDL_Rect> &source_rect) { _source_rect = source_rect; }
        void setFlipped(bool is_flipped) { _is_flipped = is_flipped; }
    };

}