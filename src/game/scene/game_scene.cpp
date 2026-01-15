#include "game_scene.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/core/context.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
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

    // 设置相机边界为地图大小
    auto map_size = level_loader.getMapSize();
    auto tile_size = level_loader.getTileSize();
    glm::vec2 world_size = glm::vec2(map_size.x * tile_size.x, map_size.y * tile_size.y);

    engine::utils::Rect camera_bounds;
    camera_bounds.position = glm::vec2(0.0f, 0.0f);
    camera_bounds.size = world_size;

    _context.getCamera().setLimitBounds(camera_bounds);
    spdlog::info("Camera bounds set to: ({}, {}) - ({}, {})",
                 camera_bounds.position.x, camera_bounds.position.y,
                 camera_bounds.size.x, camera_bounds.size.y);

    createTestObject();
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
    // testCamera();
    testObject();
    testCollisionPairs();
}

void game::scene::GameScene::clean()
{
    Scene::clean();
}

void game::scene::GameScene::createTestObject()
{
    auto test_object = std::make_unique<engine::object::GameObject>("TestObject");
    _test_object = test_object.get();

    test_object->addComponent<engine::component::TransformComponent>(glm::vec2(100.0f, 100.0f));
    test_object->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", _context.getResourceManager(), engine::utils::Alignment::CENTER);
    test_object->addComponent<engine::component::PhysicsComponent>(&_context.getPhysicsEngine(), true, 1.0f);
    test_object->addComponent<engine::component::ColliderComponent>(std::make_unique<engine::physics::AABBCollider>(glm::vec2(64.0f, 64.0f)), engine::utils::Alignment::CENTER);
    addGameObject(std::move(test_object));

    auto test_object2 = std::make_unique<engine::object::GameObject>("TestObject2");
    auto test_object2_transform = test_object2->addComponent<engine::component::TransformComponent>(glm::vec2(200.0f, 100.0f));
    test_object2_transform->setScale(glm::vec2(2.0f, 2.0f));
    test_object2->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", _context.getResourceManager(), engine::utils::Alignment::CENTER);
    test_object2->addComponent<engine::component::PhysicsComponent>(&_context.getPhysicsEngine(), false);
    test_object2->addComponent<engine::component::ColliderComponent>(std::make_unique<engine::physics::AABBCollider>(glm::vec2(128.0f, 128.0f)), engine::utils::Alignment::CENTER);
    addGameObject(std::move(test_object2));
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

void game::scene::GameScene::testObject()
{
    if (!_test_object)
    {
        return;
    }
    auto &input_manger = _context.getInputManager();
    auto *physics = _test_object->getComponent<engine::component::PhysicsComponent>();

    if (input_manger.isActionDown("move_left"))
    {
        _test_object->getComponent<engine::component::TransformComponent>()->translate(glm::vec2(-1.0f, 0.0f));
    }
    if (input_manger.isActionDown("move_right"))
    {
        _test_object->getComponent<engine::component::TransformComponent>()->translate(glm::vec2(1.0f, 0.0f));
    }
    if (input_manger.isActionDown("jump") && physics)
    {
        physics->setVelocity(glm::vec2(physics->_velocity.x, -300.0f));
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
