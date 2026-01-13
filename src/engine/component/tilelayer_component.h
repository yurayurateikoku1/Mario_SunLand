#pragma once
#include "../render/sprite.h"
#include "component.h"
#include <vector>
#include <glm/vec2.hpp>

namespace engine::render
{
    class Sprite;
}
namespace engine::core
{
    class Context;
}
namespace engine::component
{
    /// @brief 瓦片类型
    enum class TileType
    {
        /// @brief 空白
        EMPTY,
        /// @brief 普通
        NORMAL,
        /// @brief 静止可碰撞
        SOLID,
        /// @brief 单向静止可碰撞
        UNSOLID,
        /// @brief 斜坡瓦片,高度左0右1
        SLOPE_0_1,
        /// @brief 斜坡瓦片,高度右0左1
        SLOPE_1_0,
        SLOPE_0_2,
        SLOPE_2_1,
        SLOPE_1_2,
        SLOPE_2_0,
        HAZARD,
        LADDER,
    };

    /// @brief  包含单个瓦片的渲染和逻辑信息
    struct TileInfo
    {
        /// @brief 瓦片的视觉表示
        render::Sprite sprite;
        /// @brief  瓦片的逻辑类型
        TileType type;
        TileInfo(render::Sprite sprite = render::Sprite(), TileType type = TileType::EMPTY) : sprite(std::move(sprite)), type(type) {}
    };
    class TileLayerComponent : public Component
    {
        friend class engine::object::GameObject;

    private:
        glm::ivec2 _tile_size;
        glm::ivec2 _map_size;
        std::vector<TileInfo> _tiles;
        glm::vec2 _offset{0.0f, 0.0f};
        bool _is_hidden = false;

    public:
        TileLayerComponent() = default;
        TileLayerComponent(const glm::ivec2 &tile_size, const glm::ivec2 &map_size, std::vector<TileInfo> &&tiles);

        const TileInfo *getTileInfoAt(glm::vec2 pos) const;
        TileType getTileTypeAt(glm::vec2 pos) const;
        TileType getTileTypeAtWorldPos(const glm::vec2 &pos) const;

        glm::ivec2 getTileSize() const { return _tile_size; };
        glm::ivec2 getMapSize() const { return _map_size; };
        glm::vec2 geWorldSize() const { return glm::vec2(_map_size.x * _tile_size.x, _map_size.y * _tile_size.y); };
        const std::vector<TileInfo> &getTiles() const { return _tiles; };
        const glm::vec2 &getOffset() const { return _offset; };
        bool isHidden() const { return _is_hidden; };

        void setOffset(const glm::vec2 &offset) { _offset = offset; };
        void setHidden(bool is_hidden) { _is_hidden = is_hidden; };

    protected:
        void init() override;
        void update(float dt, engine::core::Context &) override {}
        void render(engine::core::Context &) override;
    };
}