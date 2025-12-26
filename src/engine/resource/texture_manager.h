#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <SDL3/SDL_render.h>
#include <glm/glm.hpp>

namespace engine::resource
{
    /// @brief 贴图管理器
    class TextureManager
    {
        friend class ResourceManager;

    public:
        explicit TextureManager(SDL_Renderer *renderer);
        TextureManager(const TextureManager &) = delete;
        TextureManager &operator=(const TextureManager &) = delete;
        TextureManager(TextureManager &&) = delete;
        TextureManager &operator=(TextureManager &&) = delete;

    private:
        /// @brief 纹理管理器析构函数
        struct SDLTextureDeleter
        {
            void operator()(SDL_Texture *texture) const
            {
                SDL_DestroyTexture(texture);
            }
        };

        /// @brief 存储文件路径和指向管理纹理贴图的指针
        std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDeleter>> _textures;

        /// @brief 指向主SDL渲染器的非拥有指针
        SDL_Renderer *_renderer{nullptr};

        /// @brief 从文件加载纹理资源
        /// @param file_path
        /// @return
        SDL_Texture *loadTexture(const std::string &file_path);

        /// @brief 卸载纹理资源
        /// @param file_path
        void unloadTexture(const std::string &file_path);

        /// @brief 获取纹理资源
        /// @param file_path
        /// @return
        SDL_Texture *getTexture(const std::string &file_path);

        /// @brief 获取纹理尺寸
        /// @param file_path
        /// @return
        glm::vec2 getTextureSize(const std::string &file_path);

        /// @brief 清除所有纹理资源
        void clearTextures();
    };
}