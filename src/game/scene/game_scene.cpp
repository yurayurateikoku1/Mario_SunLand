#include "game_scene.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/core/context.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/object/game_object.h"
#include "../../engine/render/camera.h"
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
    engine::scene::LevelLoader level_loader;
    level_loader.loadLevel("assets/maps/level1.tmj", *this);

    auto *main_layer = findGameObjectByName("main");
    if (main_layer)
    {
        auto *tile_layer = main_layer->getComponent<engine::component::TileLayerComponent>();
        if (tile_layer)
        {
            _context.getPhysicsEngine().registerCollisionTileLayer(tile_layer);
        }
    }

    _player = findGameObjectByName("player");
    if (!_player)
    {
        spdlog::error("Player not found");
        return;
    }

    auto *player_transform = _player->getComponent<engine::component::TransformComponent>();
    if (player_transform)
    {
        _context.getCamera().setTarget(player_transform);
    }

    auto world_size = main_layer->getComponent<engine::component::TileLayerComponent>()->geWorldSize();
    _context.getCamera().setLimitBounds(engine::utils::Rect(glm::vec2{0.0f}, world_size));

    _context.getPhysicsEngine().setWorldBounds(engine::utils::Rect(glm::vec2{0.0f}, world_size));
    Scene::init();
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
    testPlayer();
}

void game::scene::GameScene::clean()
{
    Scene::clean();
}

void game::scene::GameScene::testCamera()
{
    auto &camera = _context.getCamera();
    auto &input_manager = _context.getInputManager();

    if (input_manager.isActionDown("move_down"))
    {
        spdlog::info("move_down detected, camera position: {}, {}", camera.getPosition().x, camera.getPosition().y);
        camera.move(glm::vec2(0.0f, 1.0f));
    }
    if (input_manager.isActionDown("move_up"))
    {
        spdlog::info("move_up detected, camera position: {}, {}", camera.getPosition().x, camera.getPosition().y);
        camera.move(glm::vec2(0.0f, -1.0f));
    }
    if (input_manager.isActionDown("move_right"))
    {
        spdlog::info("move_right detected, camera position: {}, {}", camera.getPosition().x, camera.getPosition().y);
        camera.move(glm::vec2(1.0f, 0.0f));
    }
    if (input_manager.isActionDown("move_left"))
    {
        spdlog::info("move_left detected, camera position: {}, {}", camera.getPosition().x, camera.getPosition().y);
        camera.move(glm::vec2(-1.0f, 0.0f));
    }
}

void game::scene::GameScene::testPlayer()
{
    if (!_player)
    {
        return;
    }
    auto &input_manger = _context.getInputManager();
    auto *physics = _player->getComponent<engine::component::PhysicsComponent>();
    if (!physics)
    {
        return;
    }

    if (input_manger.isActionDown("move_left"))
    {
        physics->_velocity.x = -100.0f;
    }
    else
    {
        physics->_velocity.x *= 0.9f;
    }

    if (input_manger.isActionDown("move_right"))
    {
        physics->_velocity.x = 100.0f;
    }
    else
    {
        physics->_velocity.x *= 0.9f;
    }
    if (input_manger.isActionDown("jump"))
    {
        physics->_velocity.y = -400.0f;
    }
}

void game::scene::GameScene::testCollisionPairs()
{
    auto &collision_pairs = _context.getPhysicsEngine().getCollisionPairs();
    for (auto &collision_pair : collision_pairs)
    {
        spdlog::info("Collision pair: {}, {}", collision_pair.first->getName(), collision_pair.second->getName());
    }
}
