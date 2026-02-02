#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>

struct SDL_Renderer;
struct SDL_Texture;
struct MIX_Audio;
struct MIX_Mixer;
struct TTF_Font;

namespace engine::resource
{
    class TextureManager;
    class AudioManager;
    class FontManager;

    /// @brief 资源管理器中央控制器
    class ResourceManager
    {
    private:
        std::unique_ptr<TextureManager> _texture_manager{nullptr};
        std::unique_ptr<AudioManager> _audio_manager{nullptr};
        std::unique_ptr<FontManager> _font_manager{nullptr};

    public:
        explicit ResourceManager(SDL_Renderer *renderer);

        ~ResourceManager();
        ResourceManager(const ResourceManager &) = delete;
        ResourceManager(ResourceManager &&) = delete;
        ResourceManager &operator=(const ResourceManager &) = delete;
        ResourceManager &operator=(ResourceManager &&) = delete;

        void clear();

        MIX_Mixer *getMixer() const;

        //=====Texture=====
        SDL_Texture *loadTexture(const std::string &file_path);
        SDL_Texture *getTexture(const std::string &file_path);
        void unloadTexture(const std::string &file_path);
        glm::vec2 getTextureSize(const std::string &file_path);
        void clearTextures();

        //=====Sound=====
        MIX_Audio *loadSound(const std::string &file_path);
        MIX_Audio *getSound(const std::string &file_path);
        void unloadSound(const std::string &file_path);
        void clearSounds();

        //=====Music=====
        MIX_Audio *loadMusic(const std::string &file_path);
        MIX_Audio *getMusic(const std::string &file_path);
        void unloadMusic(const std::string &file_path);
        void clearMusics();

        //=====Font=====
        TTF_Font *loadFont(const std::string &file_path, int font_size);
        TTF_Font *getFont(const std::string &file_path, int font_size);
        void unloadFont(const std::string &file_path, int font_size);
        void clearFonts();
    };
}