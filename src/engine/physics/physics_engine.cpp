#include "physics_engine.h"
#include "collision.h"
#include <set>
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include "../component/collider_component.h"
#include "../component/tilelayer_component.h"
#include "../object/game_object.h"
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>
void engine::physics::PhysicsEngine::registerComponent(engine::component::PhysicsComponent *component)
{
    _physics_components.push_back(component);
}

void engine::physics::PhysicsEngine::unregisterComponent(engine::component::PhysicsComponent *component)
{
    auto it = std::remove(_physics_components.begin(), _physics_components.end(), component);
    _physics_components.erase(it, _physics_components.end());
}

void engine::physics::PhysicsEngine::registerCollisionTileLayer(engine::component::TileLayerComponent *tile_layer)
{
    tile_layer->setPhysicsEngine(this);
    _collision_tile_layers.push_back(tile_layer);
}

void engine::physics::PhysicsEngine::unregisterCollisionTileLayer(engine::component::TileLayerComponent *tile_layer)
{
    auto it = std::remove(_collision_tile_layers.begin(), _collision_tile_layers.end(), tile_layer);
    _collision_tile_layers.erase(it, _collision_tile_layers.end());
}

void engine::physics::PhysicsEngine::update(float dt)
{
    // 清空碰撞对
    _collision_pairs.clear();
    _tile_tigger_events.clear();
    // 更新所有物理组件
    for (auto *pc : _physics_components)
    {
        if (!pc || !pc->getEnabled())
        {
            continue;
        }

        pc->resetCollidedFlags();

        // 应用重力
        if (pc->getUseGravity())
        {
            pc->addForce(_gravity * pc->getMass());
        }
        // 更新速度
        pc->_velocity += (pc->getForce() / pc->getMass()) * dt;
        pc->clearForce();

        // 处理瓦片层碰撞
        resolveTileCollision(pc, dt);

        applyWorldBounds(pc);
    }
    //  检查碰撞
    checkObjectCollision();
    checkTileTriggers();
}

void engine::physics::PhysicsEngine::checkObjectCollision()
{
    for (size_t i = 0; i < _physics_components.size(); i++)
    {
        auto *pc_a = _physics_components[i];
        if (!pc_a || !pc_a->getEnabled())
        {
            continue;
        }

        auto *obj_a = pc_a->getOwner();
        if (!obj_a)
        {
            continue;
        }
        auto *cc_a = obj_a->getComponent<engine::component::ColliderComponent>();
        if (!cc_a || !cc_a->isActive())
        {
            continue;
        }

        for (size_t j = i + 1; j < _physics_components.size(); j++)
        {
            auto *pc_b = _physics_components[j];
            if (!pc_b || !pc_b->getEnabled())
            {
                continue;
            }
            auto *obj_a = pc_a->getOwner();
            if (!obj_a)
            {
                continue;
            }
            auto *cc_a = obj_a->getComponent<engine::component::ColliderComponent>();
            if (!cc_a)
            {
                continue;
            }
            auto *obj_b = pc_b->getOwner();
            if (!obj_b)
            {
                continue;
            }
            auto *cc_b = obj_b->getComponent<engine::component::ColliderComponent>();
            if (!cc_b || !cc_b->isActive())
            {
                continue;
            }
            if (collision::checkCollision(*cc_a, *cc_b))
            {

                if (obj_a->getTarget() != "solid" && obj_b->getTarget() == "solid")
                {
                    resolveSolidObjectCollisions(obj_a, obj_b);
                }
                else if (obj_a->getTarget() == "solid" && obj_b->getTarget() != "solid")
                {
                    resolveSolidObjectCollisions(obj_b, obj_a);
                }
                else
                {
                    _collision_pairs.emplace_back(obj_a, obj_b);
                }
            }
        }
    }
}

void engine::physics::PhysicsEngine::resolveTileCollision(engine::component::PhysicsComponent *pc, float dt)
{
    auto *obj = pc->getOwner();
    if (!obj)
    {
        return;
    }
    auto *tc = obj->getComponent<engine::component::TransformComponent>();
    auto *cc = obj->getComponent<engine::component::ColliderComponent>();
    if (!tc || !cc || cc->isTrigger())
    {
        return;
    }
    auto world_aabb = cc->getWorldAABB();
    auto obj_pos = world_aabb.position;
    auto obj_size = world_aabb.size;
    if (world_aabb.size.x <= 0.0f || world_aabb.size.y <= 0.0f)
    {
        return;
    }

    auto tolerance = 1.0f;           // 检查右边缘和下边缘要减1像素
    auto ds = pc->_velocity * dt;    // 计算物体在dt内的位移
    auto new_obj_pos = obj_pos + ds; // 计算新的位置

    if (!cc->isActive())
    {
        tc->translate(ds);
        pc->_velocity = glm::clamp(pc->_velocity, -_max_speed, _max_speed);
        return;
    }

    for (auto *layer : _collision_tile_layers)
    {
        if (!layer)
        {
            continue;
        }
        auto tile_size = layer->getTileSize();
        auto layer_offset = layer->getOffset();
        // 轴分离检测，先检查x方向是否碰撞（y方向使用初始位置 obj_pos.y）
        if (ds.x > 0.0f)
        {
            // 检查右侧碰撞,需要测试右上和右下
            auto right_x = new_obj_pos.x + obj_size.x;
            auto tile_x = static_cast<int>(std::floor((right_x - layer_offset.x) / tile_size.x));
            // y方向使用初始位置
            auto tile_y_top = static_cast<int>(std::floor((obj_pos.y - layer_offset.y) / tile_size.y));
            auto tile_y_bottom = static_cast<int>(std::floor((obj_pos.y + obj_size.y - tolerance - layer_offset.y) / tile_size.y));
            auto tile_type_top = layer->getTileTypeAt({tile_x, tile_y_top});
            auto tile_type_bottom = layer->getTileTypeAt({tile_x, tile_y_bottom});
            if (tile_type_top == engine::component::TileType::SOLID || tile_type_bottom == engine::component::TileType::SOLID)
            {
                // 速度归零，x方向移动到贴着墙的位置
                new_obj_pos.x = layer_offset.x + tile_x * tile_size.x - obj_size.x;
                pc->_velocity.x = 0.0f;
                pc->setCollidedRight(true);
            }
            else
            {
                auto width_right = new_obj_pos.x + obj_size.x - tile_x * tile_size.x;
                auto height_right = getTileHeightAtWidth(width_right, tile_type_bottom, tile_size);
                if (height_right > 0.0f)
                {
                    if (new_obj_pos.y > (tile_y_bottom + 1) * layer->getTileSize().y - obj_size.y - height_right)
                    {
                        new_obj_pos.y = (tile_y_bottom + 1) * layer->getTileSize().y - obj_size.y - height_right;
                    }
                    pc->setCollidedBelow(true);
                }
            }
        }
        else if (ds.x < 0.0f)
        {
            // 检测左侧碰撞，分别测试左上和左下
            auto left_x = new_obj_pos.x;
            auto tile_x = static_cast<int>(std::floor((left_x - layer_offset.x) / tile_size.x));
            // y方向使用初始位置
            auto tile_y_top = static_cast<int>(std::floor((obj_pos.y - layer_offset.y) / tile_size.y));
            auto tile_y_bottom = static_cast<int>(std::floor((obj_pos.y + obj_size.y - tolerance - layer_offset.y) / tile_size.y));
            auto tile_type_top = layer->getTileTypeAt({tile_x, tile_y_top});
            auto tile_type_bottom = layer->getTileTypeAt({tile_x, tile_y_bottom});
            if (tile_type_top == engine::component::TileType::SOLID || tile_type_bottom == engine::component::TileType::SOLID)
            {
                new_obj_pos.x = layer_offset.x + (tile_x + 1) * tile_size.x;
                pc->_velocity.x = 0.0f;
                pc->setCollidedLeft(true);
            }
            else
            {
                auto width_left = new_obj_pos.x - tile_x * tile_size.x;
                auto height_left = getTileHeightAtWidth(width_left, tile_type_bottom, tile_size);
                if (height_left > 0.0f)
                {
                    if (new_obj_pos.y > (tile_y_bottom + 1) * layer->getTileSize().y - obj_size.y - height_left)
                    {
                        new_obj_pos.y = (tile_y_bottom + 1) * layer->getTileSize().y - obj_size.y - height_left;
                    }
                    pc->setCollidedBelow(true);
                }
            }
        }

        // 检测y方向是否碰撞（x方向使用初始位置 obj_pos.x）
        if (ds.y > 0.0f)
        {
            // 检查底部碰撞，需要测试左下和右下
            auto bottom_y = new_obj_pos.y + obj_size.y;
            auto tile_y = static_cast<int>(std::floor((bottom_y - layer_offset.y) / tile_size.y));
            // x方向使用初始位置
            auto tile_x_left = static_cast<int>(std::floor((obj_pos.x - layer_offset.x) / tile_size.x));
            auto tile_x_right = static_cast<int>(std::floor((obj_pos.x + obj_size.x - tolerance - layer_offset.x) / tile_size.x));
            auto tile_type_left = layer->getTileTypeAt({tile_x_left, tile_y});
            auto tile_type_right = layer->getTileTypeAt({tile_x_right, tile_y});

            if (tile_type_left == engine::component::TileType::SOLID || tile_type_right == engine::component::TileType::SOLID || tile_type_left == engine::component::TileType::UNISOLID || tile_type_right == engine::component::TileType::UNISOLID)
            {
                auto corrected_y = layer_offset.y + tile_y * tile_size.y - obj_size.y;
                new_obj_pos.y = corrected_y;
                pc->_velocity.y = 0.0f;
                pc->setCollidedBelow(true);
            }
            else
            {
                auto width_left = new_obj_pos.x - tile_x_left * tile_size.x;
                auto width_right = new_obj_pos.x + obj_size.x - tile_x_right * tile_size.x;
                auto height_left = getTileHeightAtWidth(width_left, tile_type_left, tile_size);
                auto height_right = getTileHeightAtWidth(width_right, tile_type_right, tile_size);
                auto height = glm::max(height_left, height_right);
                if (height > 0.0f)
                {
                    if (new_obj_pos.y > (tile_y + 1) * layer->getTileSize().y - obj_size.y - height)
                    {
                        new_obj_pos.y = (tile_y + 1) * layer->getTileSize().y - obj_size.y - height;
                        pc->_velocity.y = 0.0f;
                        pc->setCollidedBelow(true);
                    }
                    else if (tile_type_left == engine::component::TileType::LADDER && tile_type_right == engine::component::TileType::LADDER)
                    {
                        auto tile_type_up_l = layer->getTileTypeAt({tile_x_left, tile_y - 1});
                        auto tile_type_up_r = layer->getTileTypeAt({tile_x_right, tile_y - 1});
                        if (tile_type_up_r != engine::component::TileType::LADDER && tile_type_up_l != engine::component::TileType::LADDER)
                        {
                            if (pc->getUseGravity())
                            {
                                pc->setOnTopLadder(true);
                                pc->setCollidedBelow(true);

                                new_obj_pos.y = tile_y * layer->getTileSize().y - obj_size.y;
                                pc->_velocity.y = 0.0f;
                            }
                            else
                            {
                            }
                        }
                    }
                }
            }
        }
        else if (ds.y < 0.0f)
        {
            // 检查顶部碰撞，需要测试左上和右上
            auto top_y = new_obj_pos.y;
            auto tile_y = static_cast<int>(std::floor((top_y - layer_offset.y) / tile_size.y));
            // x方向使用初始位置
            auto tile_x_left = static_cast<int>(std::floor((obj_pos.x - layer_offset.x) / tile_size.x));
            auto tile_x_right = static_cast<int>(std::floor((obj_pos.x + obj_size.x - tolerance - layer_offset.x) / tile_size.x));
            auto tile_type_left = layer->getTileTypeAt({tile_x_left, tile_y});
            auto tile_type_right = layer->getTileTypeAt({tile_x_right, tile_y});
            if (tile_type_left == engine::component::TileType::SOLID || tile_type_right == engine::component::TileType::SOLID)
            {
                new_obj_pos.y = layer_offset.y + (tile_y + 1) * tile_size.y;
                pc->_velocity.y = 0.0f;
                pc->setCollidedAbove(true);
            }
        }
    }
    // 使用 translate 移动差值，而不是直接设置位置
    // 因为 obj_pos 是 collider 的世界位置，而 tc 存储的是 transform 的位置
    tc->translate(new_obj_pos - obj_pos);
    pc->_velocity = glm::clamp(pc->_velocity, -_max_speed, _max_speed);
}

void engine::physics::PhysicsEngine::resolveSolidObjectCollisions(engine::object::GameObject *move_obj, engine::object::GameObject *solid_obj)
{
    auto *move_tc = move_obj->getComponent<engine::component::TransformComponent>();
    auto *move_pc = move_obj->getComponent<engine::component::PhysicsComponent>();
    auto *move_cc = move_obj->getComponent<engine::component::ColliderComponent>();
    auto *solid_cc = solid_obj->getComponent<engine::component::ColliderComponent>();

    auto move_aabb = move_cc->getWorldAABB();
    auto solid_aabb = solid_cc->getWorldAABB();

    auto move_center = move_aabb.position + move_aabb.size / 2.0f;
    auto solid_center = solid_aabb.position + solid_aabb.size / 2.0f;

    auto overlap = glm::vec2(move_aabb.size / 2.0f + solid_aabb.size / 2.0f) - glm::abs(move_center - solid_center);
    if (overlap.x < 0.1f && overlap.y < 0.1f)
    {
        return;
    }
    if (overlap.x < overlap.y)
    {
        if (move_center.x < solid_center.x)
        {
            move_tc->translate(glm::vec2(-overlap.x, 0.0f));
            if (move_pc->_velocity.x > 0.0f)
            {
                move_pc->_velocity.x = 0.0f;
                move_pc->setCollidedRight(true);
            }
        }
        else
        {
            move_tc->translate(glm::vec2(overlap.x, 0.0f));
            if (move_pc->_velocity.x < 0.0f)
            {
                move_pc->_velocity.x = 0.0f;
                move_pc->setCollidedLeft(true);
            }
        }
    }
    else
    {
        if (move_center.y < solid_center.y)
        {
            move_tc->translate(glm::vec2(0.0f, -overlap.y));
            if (move_pc->_velocity.y > 0.0f)
            {
                move_pc->_velocity.y = 0.0f;
                move_pc->setCollidedBelow(true);
            }
        }
        else
        {
            move_tc->translate(glm::vec2(0.0f, overlap.y));
            if (move_pc->_velocity.y < 0.0f)
            {
                move_pc->_velocity.y = 0.0f;
                move_pc->setCollidedAbove(true);
            }
        }
    }
}

void engine::physics::PhysicsEngine::applyWorldBounds(engine::component::PhysicsComponent *pc)
{
    if (!pc || !_world_bounds)
        return;

    // 只限定左、上、右边界，不限定下边界，以碰撞盒作为判断依据
    auto *obj = pc->getOwner();
    auto *cc = obj->getComponent<engine::component::ColliderComponent>();
    auto *tc = obj->getComponent<engine::component::TransformComponent>();
    auto world_aabb = cc->getWorldAABB();
    auto obj_pos = world_aabb.position;
    auto obj_size = world_aabb.size;

    // 检查左边界
    if (obj_pos.x < _world_bounds->position.x)
    {
        pc->_velocity.x = 0.0f;
        obj_pos.x = _world_bounds->position.x;
    }
    // 检查上边界
    if (obj_pos.y < _world_bounds->position.y)
    {
        pc->_velocity.y = 0.0f;
        obj_pos.y = _world_bounds->position.y;
    }
    // 检查右边界
    if (obj_pos.x + obj_size.x > _world_bounds->position.x + _world_bounds->size.x)
    {
        pc->_velocity.x = 0.0f;
        obj_pos.x = _world_bounds->position.x + _world_bounds->size.x - obj_size.x;
    }
    // 更新物体位置(使用translate方法，新位置 - 旧位置)
    tc->translate(obj_pos - world_aabb.position);
}

float engine::physics::PhysicsEngine::getTileHeightAtWidth(float width, engine::component::TileType tile_type, glm::vec2 tile_size)
{
    auto rel_x = glm::clamp(width / tile_size.x, 0.0f, 1.0f);
    switch (tile_type)
    {
    case engine::component::TileType::SLOPE_0_1:
        return rel_x * tile_size.y;
    case engine::component::TileType::SLOPE_0_2:
        return rel_x * tile_size.y * 0.5f;
    case engine::component::TileType::SLOPE_2_1:
        return rel_x * tile_size.y * 0.5f + tile_size.y * 0.5f;
    case engine::component::TileType::SLOPE_1_0:
        return (1.0f - rel_x) * tile_size.y;
    case engine::component::TileType::SLOPE_2_0:
        return (1.0f - rel_x) * tile_size.y * 0.5f;
    case engine::component::TileType::SLOPE_1_2:
        return (1.0f - rel_x) * tile_size.y * 0.5f + tile_size.y * 0.5f;
    default:
        return 0.0f;
    }
}

void engine::physics::PhysicsEngine::checkTileTriggers()
{
    for (auto *pc : _physics_components)
    {

        auto world_aabb = pc->getOwner()->getComponent<engine::component::ColliderComponent>()->getWorldAABB();

        // 使用 set 防止同一帧内因接触多个同类瓦片而重复触发
        std::set<engine::component::TileType> triggers_set;

        for (auto *layer : _collision_tile_layers)
        {
            if (!layer)
            {
                continue;
            }
            auto tile_size = layer->getTileSize();
            constexpr float tolerance = 1.0f;
            auto start_x = static_cast<int>(std::floor((world_aabb.position.x) / tile_size.x));
            auto end_x = static_cast<int>(std::ceil((world_aabb.position.x + world_aabb.size.x - tolerance) / tile_size.x));
            auto start_y = static_cast<int>(std::floor((world_aabb.position.y) / tile_size.y));
            auto end_y = static_cast<int>(std::ceil((world_aabb.position.y + world_aabb.size.y - tolerance) / tile_size.y));
            for (int x = start_x; x < end_x; ++x)
            {
                for (int y = start_y; y < end_y; ++y)
                {
                    auto tile_type = layer->getTileTypeAt({x, y});
                    if (tile_type == engine::component::TileType::HAZARD)
                    {
                        triggers_set.insert(tile_type);
                    }
                    else if (tile_type == engine::component::TileType::LADDER)
                    {
                        pc->setCollidedLoadder(true);
                    }
                }
            }
        }
        // 将本帧触发的所有唯一类型的事件记录下来
        for (const auto &type : triggers_set)
        {
            _tile_tigger_events.emplace_back(pc->getOwner(), type);
        }
    }
}
