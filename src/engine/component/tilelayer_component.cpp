#include "tilelayer_component.h"
#include "../object/game_object.h"
#include "../core/context.h"
#include "../render/render.h"
#include "../render/camera.h"
#include <spdlog/spdlog.h>

engine::component::TileLayerComponent::TileLayerComponent(const glm::ivec2 &tile_size, const glm::ivec2 &map_size, std::vector<TileInfo> &&tiles)
    : _tile_size(tile_size), _map_size(map_size), _tiles(std::move(tiles))
{
    if (_tiles.size() != static_cast<size_t>(_map_size.x * _map_size.y))
    {
        /* code */
        spdlog::error("TileLayerComponent: tiles size is not equal to map size");
        tiles.clear();
        _map_size = {0, 0};
    }
    spdlog::info("TileLayerComponent created");
}

const engine::component::TileInfo *engine::component::TileLayerComponent::getTileInfoAt(glm::vec2 pos) const
{
    if (pos.x <= 0 || pos.x >= _map_size.x || pos.y <= 0 || pos.y >= _map_size.y)
    {
        spdlog::warn("TileLayerComponent: pos out of range");
        return nullptr;
    }
    size_t index = static_cast<size_t>((pos.y) * _map_size.x + pos.x);

    if (index < _tiles.size())
    {
        /* code */
        return &_tiles[index];
    }

    return nullptr;
}

engine::component::TileType engine::component::TileLayerComponent::getTileTypeAt(glm::vec2 pos) const
{
    const TileInfo *info = getTileInfoAt(pos);
    return info ? info->type : TileType::EMPTY;
}

engine::component::TileType engine::component::TileLayerComponent::getTileTypeAtWorldPos(const glm::vec2 &pos) const
{
    glm::vec2 relative_pos = pos - _offset;
    int tile_x = static_cast<int>(std::floor(relative_pos.x / _tile_size.x));
    int tile_y = static_cast<int>(std::floor(relative_pos.y / _tile_size.y));
    return getTileTypeAt({tile_x, tile_y});
}

void engine::component::TileLayerComponent::init()
{
    if (!_owner)
    {
        /* code */
        spdlog::warn("TileLayerComponent: Owner is not set.");
    }

    spdlog::info("TileLayerComponent initialized");
}

void engine::component::TileLayerComponent::render(engine::core::Context &context)
{
    if (_tile_size.x <= 0 || _tile_size.y <= 0)
    {
        /* code */
        return;
    }

    int rendered_count = 0;
    for (int y = 0; y < _map_size.y; ++y)
    {
        for (int x = 0; x < _map_size.x; ++x)
        {
            size_t index = static_cast<size_t>(y) * _map_size.x + x;
            // 检测索引有效性以瓦片是否需要渲染
            if (index < _tiles.size() && _tiles[index].type != TileType::EMPTY)
            {
                /* code */
                const auto &tile_info = _tiles[index];
                // 计算瓦片左上角的世界坐标
                glm::vec2 tile_left_top_pos = {
                    _offset.x + static_cast<float>(x) * _tile_size.x,
                    _offset.y + static_cast<float>(y) * _tile_size.y};
                // 如果瓦片高度不等于纹理高度 调整y坐标 瓦片层的对齐点是左下角
                const auto &source_rect = tile_info.sprite.getSourceRect();
                if (source_rect.has_value() && static_cast<int>(source_rect->h) != _tile_size.y)
                {
                    /* code */
                    tile_left_top_pos.y -= (_tile_size.y - source_rect->h) - static_cast<float>(_tile_size.y);
                }
                context.getRender().drawSprite(context.getCamera(), tile_info.sprite, tile_left_top_pos);
                rendered_count++;
            }
        }
    }
}
