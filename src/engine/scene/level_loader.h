#pragma once
#include <nlohmann/json.hpp>
#include <glm/vec2.hpp>
namespace engine::component
{
    struct TileInfo;
}
namespace engine::scene
{
    class Scene;

    class LevelLoader final
    {
    public:
        LevelLoader() = default;

        bool loadLevel(const std::string &map_path, Scene &scene);

    private:
        std::string _map_path;
        glm::ivec2 _map_size;
        glm::ivec2 _tile_size;
        /// @brief 瓦片集数据
        std::map<int, nlohmann::json> _tileset_data;

        void loadImageLayer(const nlohmann::json &layer_json, Scene &scene);
        void loadTileLayer(const nlohmann::json &layer_json, Scene &scene);
        void loadObjectLayer(const nlohmann::json &layer_json, Scene &scene);

        engine::component::TileInfo getTileInfoByGid(int gid);

        void loadTileset(const std::string &tileset_path, int first_gid);

        std::string reslovePath(const std::string &relative_path, const std::string &file_path);
    };
}