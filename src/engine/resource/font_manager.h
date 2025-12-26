#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <SDL3_ttf/SDL_ttf.h>
#include <utility>
#include <functional>
#include <glm/glm.hpp>
namespace engine::resource
{
    using FontKey = std::pair<std::string, int>;
    struct FontKeyHash
    {
        std::size_t operator()(const FontKey &key) const
        {
            std::hash<std::string> str_hash;
            std::hash<int> int_hash;
            return str_hash(key.first) ^ int_hash(key.second);
        }
    };

    /// @brief 字体管理器
    class FontManager
    {
        friend class ResourceManager;

    public:
        FontManager();
        ~FontManager();
        FontManager(const FontManager &) = delete;
        FontManager(FontManager &&) = delete;
        FontManager &operator=(const FontManager &) = delete;
        FontManager &operator=(FontManager &&) = delete;

    private:
        struct SDLFontDeleter
        {
            /* data */
            void operator()(TTF_Font *font) const
            {
                if (font)
                {
                    TTF_CloseFont(font);
                }
            }
        };

        SDL_Renderer *_renderer = nullptr;
        std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFontDeleter>, FontKeyHash> _fonts;

        TTF_Font *loadFont(const std::string &file_path, int font_size);
        void unloadFont(const std::string &file_path, int font_size);
        TTF_Font *getFont(const std::string &file_path, int font_size);
        void clearFonts();
    };
}