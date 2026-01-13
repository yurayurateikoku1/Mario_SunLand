#include "level_loader.h"
#include "../component/transform_component.h"
#include "../component/sprite_component.h"
#include "../component/parallax_component.h"
#include "../component/tilelayer_component.h"
#include "../scene/scene.h"
#include "../core/context.h"
#include "../resource/resource_manager.h"
#include "../render/sprite.h"
#include "../utils/math.h"
#include "../object/game_object.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
#include <glm/vec2.hpp>
#include <filesystem>
bool engine::scene::LevelLoader::loadLevel(const std::string &level_path, Scene &scene)
{
    _map_path = level_path;
    std::ifstream map_file(level_path);
    if (!map_file.is_open())
    {
        spdlog::error("Failed to open map file: {}", level_path);
        return false;
    }
    nlohmann::json json_data;
    try
    {
        /* code */
        map_file >> json_data;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to parse map file: {},{},{},{}", level_path, e.what(), __FILE__, __LINE__);
        return false;
    }

    _map_path = level_path;
    _map_size = glm::ivec2(json_data.value("width", 0), json_data.value("height", 0));
    _tile_size = glm::ivec2(json_data.value("tilewidth", 0), json_data.value("tileheight", 0));

    if (json_data.contains("tilesets") && json_data["tilesets"].is_array())
    {
        /* code */
        for (const auto &tileset_json : json_data["tilesets"])
        {
            if (!tileset_json.contains("source") || !tileset_json["source"].is_string() ||
                !tileset_json.contains("firstgid") || !tileset_json["firstgid"].is_number_integer())
            {
                /* code */
                spdlog::error("Invalid tileset data in map file: {}", _map_path);
                continue;
            }

            auto tileset_path = reslovePath(tileset_json["source"], _map_path);
            auto first_gid = tileset_json["firstgid"];
            loadTileset(tileset_path, first_gid);
        }
    }

    if (!json_data.contains("layers") || !json_data["layers"].is_array())
    {
        /* code */
        spdlog::error("Invalid map file: {}", level_path);
        return false;
    }

    for (const auto &layer_json : json_data["layers"])
    {
        std::string layer_type = layer_json.value("type", "none");
        if (!layer_json.value("visible", true))
        {
            /* code */
            spdlog::info("Skipping invisible layer: {}", layer_type);
            continue;
        }
        if (layer_type == "imagelayer")
        {
            loadImageLayer(layer_json, scene);
        }
        else if (layer_type == "tilelayer")
        {
            loadTileLayer(layer_json, scene);
        }
        else if (layer_type == "objectgroup")
        {
            loadObjectLayer(layer_json, scene);
        }
        else
        {
            spdlog::warn("Unknown layer type: {}", layer_type);
        }
    }
    spdlog::info("Map file loaded: {}", level_path);
    return true;
}

void engine::scene::LevelLoader::loadImageLayer(const nlohmann::json &layer_json, Scene &scene)
{
    const std::string &file_path = layer_json.value("image", "");
    if (file_path.empty())
    {
        /* code */
        spdlog::warn("Image layer has no image file");
        return;
    }
    auto texture_id = reslovePath(file_path, _map_path);
    const glm::vec2 offset = glm::vec2(layer_json.value("offsetx", 0.0f), layer_json.value("offsety", 0.0f));

    const glm::vec2 scroll_factor = glm::vec2(layer_json.value("parallaxx", 1.0f), layer_json.value("parallaxy", 1.0f));
    const glm::bvec2 repeat = glm::bvec2(layer_json.value("repeatx", false), layer_json.value("repeaty", false));

    const std::string &layer_name = layer_json.value("name", "Unamed");

    auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
    game_object->addComponent<engine::component::TransformComponent>(offset);
    game_object->addComponent<engine::component::ParallaxComponent>(texture_id, scroll_factor, repeat);
    scene.addGameObject(std::move(game_object));
    spdlog::info("Image layer loaded: {}", file_path);
}
void engine::scene::LevelLoader::loadTileLayer(const nlohmann::json &layer_json, Scene &scene)
{
    if (!layer_json.contains("data") || !layer_json["data"].is_array())
    {
        spdlog::error("TileLayer data missing or not an array: {}", layer_json.value("name", "Unamed"));
        return;
    }
    std::vector<engine::component::TileInfo> tiles;
    tiles.reserve(_map_size.x * _map_size.y);

    const auto &data = layer_json["data"];
    int non_empty_count = 0;
    for (const auto &gid : data)
    {
        auto tile_info = getTileInfoByGid(gid);
        if (tile_info.type != engine::component::TileType::EMPTY)
        {
            non_empty_count++;
        }
        tiles.push_back(tile_info);
    }

    const std::string &layer_name = layer_json.value("name", "Unamed");
    auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
    game_object->addComponent<engine::component::TileLayerComponent>(_tile_size, _map_size, std::move(tiles));
    scene.addGameObject(std::move(game_object));
}

void engine::scene::LevelLoader::loadObjectLayer(const nlohmann::json &layer_json, Scene &scene)
{
}

std::string engine::scene::LevelLoader::reslovePath(const std::string &relative_path, const std::string &file_path)
{
    try
    {
        auto map_dir = std::filesystem::path(file_path).parent_path();
        auto final_path = std::filesystem::canonical(map_dir / relative_path);

        return final_path.string();
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to resolve path: {},{},{},{}", file_path, e.what(), __FILE__, __LINE__);
        return file_path;
    }
}

engine::component::TileInfo engine::scene::LevelLoader::getTileInfoByGid(int gid)
{
    if (gid == 0)
    {
        return engine::component::TileInfo();
    }
    auto tileset_it = _tileset_data.upper_bound(gid);
    if (tileset_it == _tileset_data.begin())
    {
        spdlog::error("Failed to find tileset for gid: {}", gid);
        return engine::component::TileInfo();
    }

    --tileset_it;
    const auto &tileset = tileset_it->second;
    auto local_id = gid - tileset_it->first;
    const std::string &file_path = tileset.value("file_path", "");
    if (file_path.empty())
    {
        spdlog::error("Tileset file path not found for gid: {}", gid);
        return engine::component::TileInfo();
    }

    if (tileset.contains("image"))
    {
        auto texture_id = reslovePath(tileset["image"].get<std::string>(), file_path);
        auto columns = tileset["columns"].get<int>();
        auto tile_width = tileset.value("tilewidth", _tile_size.x);
        auto tile_height = tileset.value("tileheight", _tile_size.y);

        auto coordinate_x = local_id % columns;
        auto coordinate_y = local_id / columns;

        SDL_Rect texture_rect = {
            coordinate_x * tile_width,
            coordinate_y * tile_height,
            tile_width,
            tile_height};

        engine::render::Sprite sprite{texture_id, std::make_optional(texture_rect), false};
        return engine::component::TileInfo{sprite, engine::component::TileType::NORMAL};
    }
    else
    {
        if (!tileset.contains("tiles"))
        {
            spdlog::error("Tileset file {} does not contain tiles", tileset_it->first);
            return engine::component::TileInfo();
        }
        const auto &tiles_json = tileset["tiles"];
        for (const auto &tile : tiles_json)
        {
            auto tile_id = tile.value("id", 0);
            if (tile_id == local_id)
            {
                if (!tiles_json.contains("image"))
                {
                    spdlog::error("Tileset file {} does not contain image", tileset_it->first, tile_id);
                    return engine::component::TileInfo();
                }
                auto texture_id = reslovePath(tiles_json["image"].get<std::string>(), file_path);

                auto image_width = tiles_json.value("imageheight", 0);
                auto image_height = tiles_json.value("imagewidth", 0);

                SDL_Rect texture_rect = {
                    tiles_json.value("x", 0),
                    tiles_json.value("y", 0),
                    tiles_json.value("width", image_width),
                    tiles_json.value("height", image_height)};

                engine::render::Sprite sprite{texture_id, std::make_optional(texture_rect), false};
                return engine::component::TileInfo(sprite, engine::component::TileType::NORMAL);
            }
        }
    }
    spdlog::error("Failed to find tile info for gid: {}", gid);
    return engine::component::TileInfo();
}

void engine::scene::LevelLoader::loadTileset(const std::string &tileset_path, int first_gid)
{
    std::ifstream tileset_file(tileset_path);
    if (!tileset_file.is_open())
    {
        /* code */
        spdlog::error("Failed to open tileset file: {}", tileset_path);
        return;
    }

    nlohmann::json ts_json;
    try
    {
        tileset_file >> ts_json;
    }
    catch (const nlohmann::json::exception &e)
    {
        spdlog::error("Failed to parse tileset file: {},{},{},{}", tileset_path, e.what(), __FILE__, __LINE__);
        return;
    }
    ts_json["file_path"] = tileset_path;
    _tileset_data[first_gid] = std::move(ts_json);
    spdlog::info("Tileset loaded: {},first_gid:{}", tileset_path, first_gid);
}
