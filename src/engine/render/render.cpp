#include "render.h"
#include "../resource/resource_manager.h"
#include "camera.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>
#include <stdexcept>
engine::render::Renderer::Renderer(SDL_Renderer *sdl_renderer, engine::resource::ResourceManager *resource_manager)
    : _renderer(sdl_renderer), _resource_manager(resource_manager)
{
    spdlog::info("Renderer init ...");
    if (!_renderer)
    {
        throw std::runtime_error("Renderer init failed");
    }
    if (!_resource_manager)
    {
        throw std::runtime_error("Renderer init failed");
    }

    spdlog::info("Renderer init successfully");
}

void engine::render::Renderer::drawSprite(const Camera &camera, const engine::render::Sprite &sprite, const glm::vec2 &position, const glm::vec2 &scale, double angle)
{
    auto texture = _resource_manager->getTexture(sprite.getTextureId());
    if (!texture)
    {
        spdlog::error("Texture not found:{}", sprite.getTextureId());
        return;
    }
    auto src_rect = getSpriteSrcRect(sprite);
    if (!src_rect.has_value())
    {
        spdlog::error("Invalid source rectangle:{}", sprite.getTextureId());
        return;
    }

    glm::vec2 position_screen = camera.world2Screen(position);
    float scale_w = src_rect.value().w * scale.x;
    float scale_h = src_rect.value().h * scale.y;
    SDL_FRect dest_rect = {position_screen.x, position_screen.y, scale_w, scale_h};
    if (!isRectInViewport(camera, dest_rect))
    {
        return;
    }

    if (!SDL_RenderTextureRotated(_renderer, texture, &src_rect.value(), &dest_rect, angle, NULL, sprite.isFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE))
    {
        spdlog::error("Render texture failed:{},{}", sprite.getTextureId(), SDL_GetError());
    }
}

void engine::render::Renderer::drawParallx(const Camera &camera, const engine::render::Sprite &sprite, const glm::vec2 &position, const glm::vec2 &scroll_factor, const glm::bvec2 &repeat, const glm::vec2 &scale)
{
    auto texture = _resource_manager->getTexture(sprite.getTextureId());
    if (!texture)
    {
        spdlog::error("Texture not found:{}", sprite.getTextureId());
        return;
    }
    auto src_rect = getSpriteSrcRect(sprite);
    if (!src_rect.has_value())
    {
        spdlog::error("Invalid source rectangle:{}", sprite.getTextureId());
        return;
    }
    glm::vec2 position_screen = camera.world2ScreenWithParallax(position, scroll_factor);
    float scale_w = src_rect.value().w * scale.x;
    float scale_h = src_rect.value().h * scale.y;
    glm::vec2 start;
    glm::vec2 stop;
    glm::vec2 viewport_size = camera.getViewportSize();
    if (repeat.x)
    {
        start.x = glm::mod(position_screen.x, scale_w) - scale_w;
        stop.x = viewport_size.x;
    }
    else
    {
        start.x = position_screen.x;
        stop.x = glm::min(position_screen.x + scale_w, viewport_size.x); // 结束点是一个纹理宽度之后，但不超过视口宽度
    }
    if (repeat.y)
    {
        start.y = glm::mod(position_screen.y, scale_h) - scale_h;
        stop.y = viewport_size.y;
    }
    else
    {
        start.y = position_screen.y;
        stop.y = glm::min(position_screen.y + scale_h, viewport_size.y);
    }

    for (float y = start.y; y < stop.y; y += scale_h)
    {
        for (float x = start.x; x < stop.x; x += scale_w)
        {
            SDL_FRect dest_rect = {x, y, scale_w, scale_h};
            if (!SDL_RenderTexture(_renderer, texture, nullptr, &dest_rect))
            {
                spdlog::error("Render texture failed:{},{}", sprite.getTextureId(), SDL_GetError());
                return;
            }
        }
    }
}

std::optional<SDL_FRect> engine::render::Renderer::getSpriteSrcRect(const Sprite &sprite)
{
    SDL_Texture *texture = _resource_manager->getTexture(sprite.getTextureId());
    if (!texture)
    {
        spdlog::error("Texture not found: {}", sprite.getTextureId());
        return std::nullopt;
    }
    auto src_rect = sprite.getSourceRect();
    if (src_rect.has_value())
    {
        if (src_rect.value().w <= 0 || src_rect.value().h <= 0)
        {
            spdlog::error("Invalid source rectangle:{}", sprite.getTextureId());
            return std::nullopt;
        }
    }
    else
    {
        SDL_FRect result = {0, 0, 0, 0};
        if (!SDL_GetTextureSize(texture, &result.w, &result.h))
        {
            spdlog::error("Get texture size failed:{}", sprite.getTextureId());
            return std::nullopt;
        }
        return result;
    }

    return std::optional<SDL_FRect>();
}

bool engine::render::Renderer::isRectInViewport(const Camera &camera, const SDL_FRect &rect)
{
    glm::vec2 viewport_size = camera.getViewportSize();
    return rect.x + rect.w >= 0 && rect.x <= viewport_size.x && rect.y + rect.h >= 0 && rect.y <= viewport_size.y;
}
