#include "level_loader.h"
#include "../component/transform_component.h"
#include "../component/sprite_component.h"
#include "../component/parallax_component.h"
#include "../component/tilelayer_component.h"
#include "../component/collider_component.h"
#include "../component/physics_component.h"
#include "../component/animation_component.h"
#include "../../engine/render/animation.h"
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
    const glm::vec2 offset = glm::vec2(layer_json.value("offsetx", 0.0f), layer_json.value("offsety", 0.0f));

    // 统计 SOLID 瓦片数量
    int solid_count = 0;
    for (const auto &tile : tiles)
    {
        if (tile.type == engine::component::TileType::SOLID)
        {
            solid_count++;
        }
    }
    spdlog::info("TileLayer '{}': total={}, non_empty={}, solid={}", layer_name, tiles.size(), non_empty_count, solid_count);

    auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
    auto *tile_layer = game_object->addComponent<engine::component::TileLayerComponent>(_tile_size, _map_size, std::move(tiles));
    tile_layer->setOffset(offset);
    scene.addGameObject(std::move(game_object));
}

void engine::scene::LevelLoader::loadObjectLayer(const nlohmann::json &layer_json, Scene &scene)
{
    spdlog::info("Loading object layer: {}", layer_json.value("name", "Unamed"));

    if (!layer_json.contains("objects") || !layer_json["objects"].is_array())
    {
        spdlog::error("ObjectLayer objects missing or not an array: {}", layer_json.value("name", "Unamed"));
        return;
    }

    const auto &objects = layer_json["objects"];
    spdlog::info("Found {} objects in layer", objects.size());

    for (const auto &object : objects)
    {
        /* code */
        auto gid = object.value("gid", 0);
        const std::string &object_name = object.value("name", "Unamed");
        spdlog::info("Processing object: {} (gid: {})", object_name, gid);

        // 如果gid为0 代表绘制的形状 可能是碰撞盒 触发器
        if (gid == 0)
        {
        }
        else
        {
            auto tile_info = getTileInfoByGid(gid);
            if (tile_info.sprite.getTextureId().empty())
            {
                spdlog::error("Failed to find tileset for gid: {}", gid);
                continue;
            }
            auto position = glm::vec2(object.value("x", 0.0f), object.value("y", 0.0f));
            auto dst_size = glm::vec2(object.value("width", 0.0f), object.value("height", 0.0f));

            spdlog::info("Object {} original position: ({}, {}), size: ({}, {})",
                         object_name, position.x, position.y, dst_size.x, dst_size.y);

            // Tiled 对象的 Y 坐标是底部位置，转换为顶部位置
            position = glm::vec2(position.x, position.y - dst_size.y);

            auto rotation = object.value("rotation", 0.0f);
            auto src_size_opt = tile_info.sprite.getSourceRect();
            if (!src_size_opt)
            {
                spdlog::error("Failed to find source rect for gid: {}", gid);
                continue;
            }

            auto src_size = glm::vec2(src_size_opt->w, src_size_opt->h);
            auto scale = dst_size / src_size;

            spdlog::info("Object {} adjusted position: ({}, {}), scale: ({}, {}), src_size: ({}, {})",
                         object_name, position.x, position.y, scale.x, scale.y, src_size.x, src_size.y);

            auto game_object = std::make_unique<engine::object::GameObject>(object_name);
            game_object->addComponent<engine::component::TransformComponent>(position, scale, rotation);
            game_object->addComponent<engine::component::SpriteComponent>(std::move(tile_info.sprite), scene.getContext().getResourceManager());

            auto tile_json_opt = getTileJsonByGid(gid);
            if (!tile_json_opt)
                continue;
            const auto &tile_json = tile_json_opt.value();

            if (tile_info.type == engine::component::TileType::SOLID)
            {
                auto collider = std::make_unique<engine::physics::AABBCollider>(src_size);
                game_object->addComponent<engine::component::ColliderComponent>(std::move(collider));

                game_object->addComponent<engine::component::PhysicsComponent>(&scene.getContext().getPhysicsEngine(), false);
                game_object->setTarget("solid");
            }
            else if (auto rect = getColliderRect(tile_json); rect)
            {
                auto collider = std::make_unique<engine::physics::AABBCollider>(rect->size);
                auto *cc = game_object->addComponent<engine::component::ColliderComponent>(std::move(collider));
                cc->setOffset(rect->position);
                game_object->addComponent<engine::component::PhysicsComponent>(&scene.getContext().getPhysicsEngine(), false);
            }

            auto tag = getTileProperty<std::string>(tile_json, "tag");
            if (tag)
            {
                game_object->setTarget(tag.value());
            }

            auto gravity = getTileProperty<bool>(tile_json, "gravity");
            if (gravity)
            {
                auto *pc = game_object->getComponent<engine::component::PhysicsComponent>();
                if (pc)
                {
                    pc->setUseGravity(gravity.value());
                }
                else
                {
                    spdlog::warn("Object {} has no physics component", object_name);
                    game_object->addComponent<engine::component::PhysicsComponent>(&scene.getContext().getPhysicsEngine(), gravity.value());
                }
            }

            // 获取动画信息并设置
            auto anim_string = getTileProperty<std::string>(tile_json, "animation");
            if (anim_string)
            {
                nlohmann::json anim_json;
                try
                {
                    anim_json = nlohmann::json::parse(anim_string.value());
                }
                catch (const nlohmann::json::exception &e)
                {
                    spdlog::error("Failed to parse animation json: {},{},{},{}", anim_string.value(), e.what(), __FILE__, __LINE__);
                    continue;
                }
                auto *ac = game_object->addComponent<engine::component::AnimationComponent>();
                addAnimation(anim_json, ac, src_size);
            }

            scene.addGameObject(std::move(game_object));
            spdlog::info("Object loaded: {}", object_name);
        }
    }
}

void engine::scene::LevelLoader::addAnimation(const nlohmann::json &anim_json, engine::component::AnimationComponent *ac, const glm::vec2 &sprite_size)
{
    if (!anim_json.is_object() || !ac)
    {
        spdlog::error("Invalid animation json or animation component");
        return;
    }

    for (const auto &anim : anim_json.items())
    {
        const std::string &anim_name = anim.key();
        const auto anim_info = anim.value();
        if (!anim_info.is_object())
        {
            spdlog::warn("Invalid animation json: {}", anim_name);
            continue;
        }
        auto duration_ms = anim_info.value("duration", 100);
        auto duration = static_cast<float>(duration_ms) / 1000.0f;
        auto row = anim_info.value("row", 0);

        if (!anim_info.contains("frames") || !anim_info["frames"].is_array())
        {
            spdlog::warn("Invalid animation json: {}", anim_name);
            continue;
        }

        auto animation = std::make_unique<engine::render::Animation>(anim_name);

        for (const auto &frame : anim_info["frames"])
        {
            if (!frame.is_number_integer())
            {
                spdlog::warn("Invalid animation json: {}", anim_name);
                continue;
            }
            auto column = frame.get<int>();
            SDL_Rect src_rect =
                {
                    column * sprite_size.x, row * sprite_size.y, sprite_size.x, sprite_size.y};
            animation->addFrame(src_rect, duration);
        }

        ac->addAnimation(std::move(animation));
    }
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

std::optional<engine::utils::Rect> engine::scene::LevelLoader::getColliderRect(const nlohmann::json &tile_json)
{
    if (!tile_json.contains("objectgroup"))
    {
        return std::nullopt;
    }
    auto &object_group = tile_json["objectgroup"];
    if (!object_group.contains("objects"))
    {
        return std::nullopt;
    }
    auto &objects = object_group["objects"];
    for (const auto &object : object_group["objects"])
    {
        auto rect = engine::utils::Rect(glm::vec2(object.value("x", 0.0f), object.value("y", 0.0f)), glm::vec2(object.value("width", 0.0f), object.value("height", 0.0f)));
        if (rect.size.x > 0 && rect.size.y > 0)
        {
            return rect;
        }
    }
    return std::nullopt;
}

std::optional<nlohmann::json> engine::scene::LevelLoader::getTileJsonByGid(int gid) const
{
    auto tileset_it = _tileset_data.upper_bound(gid);
    if (tileset_it == _tileset_data.begin())
    {
        spdlog::error("Failed to find tileset for gid: {}", gid);
        return std::nullopt;
    }
    --tileset_it;
    const auto &tileset = tileset_it->second;
    auto local_id = gid - tileset_it->first;
    if (!tileset.contains("tiles"))
    {
        spdlog::error("Tileset tiles not found for gid: {}", gid);
        return std::nullopt;
    }
    const auto &tileset_json = tileset["tiles"];
    for (const auto &tile_json : tileset_json)
    {
        auto tile_id = tile_json.value("id", 0);
        if (tile_id == local_id)
        {
            return tile_json;
        }
    }
    return std::nullopt;
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
        auto tile_type = getTileTypeById(tileset, local_id);
        return engine::component::TileInfo(sprite, tile_type);
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
                if (!tile.contains("image"))
                {
                    spdlog::error("Tile {} in tileset {} does not contain image", tile_id, tileset_it->first);
                    return engine::component::TileInfo();
                }
                auto texture_id = reslovePath(tile["image"].get<std::string>(), file_path);

                // 使用 tile 的 width 和 height，而不是整个图像的大小
                auto tile_width = tile.value("width", _tile_size.x);
                auto tile_height = tile.value("height", _tile_size.y);

                SDL_Rect texture_rect = {
                    0,
                    0,
                    tile_width,
                    tile_height};

                engine::render::Sprite sprite{texture_id, std::make_optional(texture_rect), false};
                auto tile_type = getTileType(tile);
                return engine::component::TileInfo(sprite, tile_type);
            }
        }
    }
    spdlog::error("Failed to find tile info for gid: {}", gid);
    return engine::component::TileInfo();
}

engine::component::TileType engine::scene::LevelLoader::getTileType(const nlohmann::json &tile_json)
{
    if (tile_json.contains("properties"))
    {
        auto &properties = tile_json["properties"];
        for (auto &property : properties)
        {
            if (property.contains("name") && property["name"] == "solid")
            {
                auto is_solid = property.value("value", false);
                return is_solid ? engine::component::TileType::SOLID : engine::component::TileType::NORMAL;
            }
            else if (property.contains("name") && property["name"] == "slope")
            {
                auto slope_type = property.value("value", "");
                if (slope_type == "0_1")
                {
                    return engine::component::TileType::SLOPE_0_1;
                }
                else if (slope_type == "1_0")
                {
                    return engine::component::TileType::SLOPE_1_0;
                }
                else if (slope_type == "0_2")
                {
                    return engine::component::TileType::SLOPE_0_2;
                }
                else if (slope_type == "2_0")
                {
                    return engine::component::TileType::SLOPE_2_0;
                }
                else if (slope_type == "2_1")
                {
                    return engine::component::TileType::SLOPE_2_1;
                }
                else if (slope_type == "1_2")
                {
                    return engine::component::TileType::SLOPE_1_2;
                }
                else
                {
                    spdlog::error("Failed to find slope type:{}", slope_type);
                    return engine::component::TileType::NORMAL;
                }
            }
            else if (property.contains("name") && property["name"] == "unisolid")
            {
                auto is_unisolid = property.value("value", false);
                return is_unisolid ? engine::component::TileType::UNISOLID : engine::component::TileType::NORMAL;
            }
        }
    }
    return engine::component::TileType::NORMAL;
}

engine::component::TileType engine::scene::LevelLoader::getTileTypeById(const nlohmann::json &tile_json, int local_id)
{
    if (tile_json.contains("tiles"))
    {
        auto &tiles = tile_json["tiles"];
        for (auto &tile : tiles)
        {
            if (tile.contains("id") && tile["id"] == local_id)
            {
                return getTileType(tile);
            }
        }
    }

    return engine::component::TileType::NORMAL;
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
