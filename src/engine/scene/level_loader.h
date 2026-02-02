#pragma once
#include <nlohmann/json.hpp>
#include <glm/vec2.hpp>
namespace engine::component
{
    struct TileInfo;
    enum class TileType;
    class AnimationComponent;
    class AudioComponent;
}
namespace engine::utils
{
    struct Rect;
}
namespace engine::scene
{
    class Scene;

    class LevelLoader final
    {
    public:
        LevelLoader() = default;

        bool loadLevel(const std::string &map_path, Scene &scene);

        const glm::ivec2 &getMapSize() const { return _map_size; }
        const glm::ivec2 &getTileSize() const { return _tile_size; }

    private:
        std::string _map_path;
        glm::ivec2 _map_size;
        glm::ivec2 _tile_size;
        /// @brief 瓦片集数据
        std::map<int, nlohmann::json> _tileset_data;

        void loadImageLayer(const nlohmann::json &layer_json, Scene &scene);
        void loadTileLayer(const nlohmann::json &layer_json, Scene &scene);
        void loadObjectLayer(const nlohmann::json &layer_json, Scene &scene);

        void addAnimation(const nlohmann::json &anim_json, engine::component::AnimationComponent *ac, const glm::vec2 &sprite_size);

        void addSound(const nlohmann::json &sound_json, engine::component::AudioComponent *ac);

        /// @brief 从瓦片json中获取属性
        /// @tparam T
        /// @param tile_json
        /// @param property_name
        /// @return
        template <typename T>
        std::optional<T> getTileProperty(const nlohmann::json &tile_json, const std::string &property_name)
        {
            if (!tile_json.contains("properties"))
            {
                return std::nullopt;
            }

            const auto &properties = tile_json["properties"];
            for (const auto &property : properties)
            {
                if (property.contains("name") && property["name"] == property_name)
                {
                    if (property.contains("value"))
                    {
                        return property["value"].get<T>();
                    }
                }
            }
            return std::nullopt;
        }

        /// @brief  从瓦片json中获取碰撞矩形
        /// @param tile_json
        /// @return
        std::optional<engine::utils::Rect> getColliderRect(const nlohmann::json &tile_json);

        /// @brief  获取瓦片json
        /// @param gid
        /// @return
        std::optional<nlohmann::json> getTileJsonByGid(int gid) const;

        engine::component::TileInfo getTileInfoByGid(int gid);

        engine::component::TileType getTileType(const nlohmann::json &tile_json);
        engine::component::TileType getTileTypeById(const nlohmann::json &tile_json, int local_id);

        void loadTileset(const std::string &tileset_path, int first_gid);

        std::string reslovePath(const std::string &relative_path, const std::string &file_path);
    };
}