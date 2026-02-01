#include "game_scene.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/core/context.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include "../component/player_component.h"
#include "../../engine/object/game_object.h"
#include "../../engine/render/camera.h"
#include "../../engine/render/animation.h"
#include "../../engine/physics/physics_engine.h"
#include "../../engine/physics/collider.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_rect.h>
#include <iostream>
game::scene::GameScene::GameScene(const std::string &name, engine::core::Context &context, engine::scene::SceneManager &scene_manager)
    : Scene(name, context, scene_manager)
{
    spdlog::info("GameScene created");
}

void game::scene::GameScene::init()
{
    if (_is_initialized)
    {
        spdlog::warn("GameScene is already initialized");
        return;
    }
    spdlog::info("GameScene initializing ...");

    if (!initlevel())
    {
        spdlog::error("GameScene initlevel failed");
        _context.getInputManager().setShouldQuit(true);
        return;
    }

    if (!initplayer())
    {
        spdlog::error("GameScene initplayer failed");
        _context.getInputManager().setShouldQuit(true);
        return;
    }

    if (!initEnemyAndItem())
    {
        spdlog::error("GameScene initEnemyAndItem failed");
        _context.getInputManager().setShouldQuit(true);
        return;
    }

    Scene::init();
    spdlog::info("GameScene initialized");
}

void game::scene::GameScene::update(float dt)
{
    Scene::update(dt);
}

void game::scene::GameScene::render()
{
    Scene::render();
}

void game::scene::GameScene::handleInput()
{
    Scene::handleInput();
    testHealth();
}

void game::scene::GameScene::clean()
{
    Scene::clean();
}

bool game::scene::GameScene::initlevel()
{
    engine::scene::LevelLoader level_loader;
    if (!level_loader.loadLevel("assets/maps/level1.tmj", *this))
    {
        spdlog::error("Failed to load level");
        return false;
    }

    auto *main_layer = findGameObjectByName("main");
    if (!main_layer)
    {
        spdlog::error("Failed to find main layer");
        return false;
    }

    auto *tile_layer = main_layer->getComponent<engine::component::TileLayerComponent>();
    if (!tile_layer)
    {
        spdlog::error("Failed to find tile layer component");
        return false;
    }
    _context.getPhysicsEngine().registerCollisionTileLayer(tile_layer);
    spdlog::info("GameScene initlevel success");

    auto world_size = main_layer->getComponent<engine::component::TileLayerComponent>()->geWorldSize();
    _context.getCamera().setLimitBounds(engine::utils::Rect(glm::vec2(0, 0), world_size));

    _context.getPhysicsEngine().setWorldBounds(engine::utils::Rect(glm::vec2(0, 0), world_size));

    return true;
}

bool game::scene::GameScene::initplayer()
{
    _player = findGameObjectByName("player");
    if (!_player)
    {
        spdlog::error("Failed to find player");
        return false;
    }

    auto *player_component = _player->addComponent<game::component::PlayerComponent>();
    if (!player_component)
    {
        spdlog::error("Failed to add player component");
        return false;
    }

    auto *player_transform = _player->getComponent<engine::component::TransformComponent>();
    if (!player_transform)
    {
        spdlog::error("Failed to find player transform component");
        return false;
    }
    _context.getCamera().setTarget(player_transform);

    return true;
}

bool game::scene::GameScene::initEnemyAndItem()
{
    bool success = true;
    for (auto &game_object : _game_objects)
    {
        if (game_object->getName() == "eagle")
        {
            if (auto *ac = game_object->getComponent<engine::component::AnimationComponent>(); ac)
            {
                ac->playAnimation("fly");
            }
            else
            {
                spdlog::error("Failed to find eagle animation component");
                success = false;
            }
        }
        if (game_object->getName() == "frog")
        {
            if (auto *ac = game_object->getComponent<engine::component::AnimationComponent>(); ac)
            {
                /* code */
                ac->playAnimation("idle");
            }
            else
            {
                spdlog::error("Failed to find frog animation component");
                success = false;
            }
        }
        if (game_object->getName() == "opossum")
        {
            if (auto *ac = game_object->getComponent<engine::component::AnimationComponent>(); ac)
            {
                /* code */
                ac->playAnimation("walk");
            }
            else
            {
                spdlog::error("Failed to find opossum animation component");
                success = false;
            }
        }
        if (game_object->getTarget() == "item")
        {
            if (auto *ac = game_object->getComponent<engine::component::AnimationComponent>(); ac)
            {
                /* code */
                ac->playAnimation("idle");
            }
            else
            {
                spdlog::error("Failed to find item animation component");
                success = false;
            }
        }
    }

    return success;
}

void game::scene::GameScene::testHealth()
{
    auto input_manager = _context.getInputManager();
    if (input_manager.isActionPressed("attack"))
    {
        _player->getComponent<game::component::PlayerComponent>()->takeDamage(1);
    }
}
