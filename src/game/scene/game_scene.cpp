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
#include "../../engine/render/text_renderer.h"
#include "../component/ai_component.h"
#include "../component/ai/patrol_behavior.h"
#include "../component/ai/jump_behavior.h"
#include "../component/ai/updown_behavior.h"
#include "../../engine/physics/physics_engine.h"
#include "../../engine/physics/collider.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/scene/scene_manager.h"
#include "../data/session_data.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_rect.h>
#include <iostream>
game::scene::GameScene::GameScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager, std::shared_ptr<game::data::SessionData> session_data)
    : Scene("GameScene", context, scene_manager), _game_session_data(std::move(session_data))
{
    if (!_game_session_data)
    {
        _game_session_data = std::make_shared<game::data::SessionData>();
    }

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
    _context.getAudioPlayer().setMusicVolume(0.2f);
    _context.getAudioPlayer().setSoundVolume(0.3f);
    _context.getAudioPlayer().playMusic("assets/audio/hurry_up_and_run.ogg", true, 1000);
    Scene::init();
    spdlog::info("GameScene initialized");
}

void game::scene::GameScene::update(float dt)
{
    Scene::update(dt);
    handleObjectCollisions();
    handleTileTriggers();
}

void game::scene::GameScene::render()
{
    Scene::render();
    testTextRenderer();
}

void game::scene::GameScene::handleInput()
{
    Scene::handleInput();
}

void game::scene::GameScene::clean()
{
    Scene::clean();
}

bool game::scene::GameScene::initlevel()
{
    engine::scene::LevelLoader level_loader;
    auto level_path = _game_session_data->getMapPath();
    if (!level_loader.loadLevel(level_path, *this))
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

    if (auto health_component = _player->getComponent<engine::component::HealthComponent>(); health_component)
    {
        health_component->setMaxHealth(_game_session_data->getMaxHealth());
        health_component->setCurrentHealth(_game_session_data->getCurrentHealth());
    }
    else
    {
        spdlog::error("Failed to find health component");
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
            if (auto *ai_component = game_object->addComponent<game::component::AIComponent>(); ai_component)
            {
                auto y_max = game_object->getComponent<engine::component::TransformComponent>()->getPosition().y;
                auto y_min = y_max - 80.0f;
                ai_component->setBehavior(std::make_unique<game::component::ai::UpDownBehavior>(y_min, y_max));
            }
        }
        if (game_object->getName() == "frog")
        {
            if (auto *ai_component = game_object->addComponent<game::component::AIComponent>(); ai_component)
            {
                auto x_max = game_object->getComponent<engine::component::TransformComponent>()->getPosition().x - 10.0f;
                auto x_min = x_max - 90.0f;
                ai_component->setBehavior(std::make_unique<game::component::ai::JumpBehavior>(x_min, x_max));
            }
        }
        if (game_object->getName() == "opossum")
        {
            if (auto *ai_component = game_object->addComponent<game::component::AIComponent>(); ai_component)
            {
                auto x_max = game_object->getComponent<engine::component::TransformComponent>()->getPosition().x;
                auto x_min = x_max - 200.0f;
                ai_component->setBehavior(std::make_unique<game::component::ai::PatrolBehavior>(x_min, x_max));
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

void game::scene::GameScene::handleObjectCollisions()
{
    auto collision_pairs = _context.getPhysicsEngine().getCollisionPairs();
    for (const auto &pair : collision_pairs)
    {
        auto objcet1 = pair.first;
        auto objcet2 = pair.second;

        if (objcet1->getName() == "player" && objcet2->getTarget() == "enemy")
        {
            playerVsEnemyCollision(objcet1, objcet2);
        }
        else if (objcet2->getName() == "player" && objcet1->getTarget() == "enemy")
        {
            playerVsEnemyCollision(objcet2, objcet1);
        }
        else if (objcet1->getName() == "player" && objcet2->getTarget() == "item")
        {
            playerVsItemCollision(objcet1, objcet2);
        }
        else if (objcet2->getName() == "player" && objcet1->getTarget() == "item")
        {
            playerVsItemCollision(objcet2, objcet1);
        }
        else if (objcet1->getName() == "player" && objcet2->getTarget() == "hazard")
        {
            objcet1->getComponent<game::component::PlayerComponent>()->takeDamage(1); // 玩家受到1点伤害
        }
        else if (objcet2->getName() == "player" && objcet1->getTarget() == "hazard")
        {
            objcet2->getComponent<game::component::PlayerComponent>()->takeDamage(1);
        }
        else if (objcet1->getName() == "player" && objcet2->getTarget() == "next_level")
        {
            toNextLevel(objcet2);
        }
        else if (objcet2->getName() == "player" && objcet1->getTarget() == "next_level")
        {
            toNextLevel(objcet1);
        }
    }
}

void game::scene::GameScene::playerVsEnemyCollision(engine::object::GameObject *player, engine::object::GameObject *enemy)
{
    // --- 踩踏判断逻辑：1. 玩家中心点在敌人上方    2. 重叠区域：overlap.x > overlap.y
    auto player_aabb = player->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    auto enemy_aabb = enemy->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    auto player_center = player_aabb.position + player_aabb.size / 2.0f;
    auto enemy_center = enemy_aabb.position + enemy_aabb.size / 2.0f;
    auto overlap = glm::vec2(player_aabb.size / 2.0f + enemy_aabb.size / 2.0f) - glm::abs(player_center - enemy_center);

    // 踩踏判断成功，敌人受伤
    if (overlap.x > overlap.y && player_center.y < enemy_center.y)
    {
        spdlog::info("player {} attack {}", player->getName(), enemy->getName());
        auto enemy_health = enemy->getComponent<engine::component::HealthComponent>();
        if (!enemy_health)
        { /* ... */
            return;
        }

        enemy_health->takeDamage(1); // 造成1点伤害
        if (!enemy_health->isAlive())
        {
            enemy->setNeedRemove(true);                     // 标记敌人为待删除状态
            createEffect(enemy_center, enemy->getTarget()); // 创建（死亡）特效
        }
        // 玩家跳起效果
        player->getComponent<engine::component::PhysicsComponent>()->_velocity.y = -300.0f;
        _context.getAudioPlayer().playSound("assets/audio/punch2a.mp3");
        _game_session_data->addScore(10);
    }
    // 踩踏判断失败，玩家受伤
    else
    {
        player->getComponent<game::component::PlayerComponent>()->takeDamage(1);
    }
}

void game::scene::GameScene::playerVsItemCollision(engine::object::GameObject *player, engine::object::GameObject *item)
{
    if (item->getName() == "fruit")
    {
        player->getComponent<engine::component::HealthComponent>()->heal(1); // 加血
    }
    else if (item->getName() == "gem")
    {
        _game_session_data->addScore(5);
    }
    item->setNeedRemove(true); // 标记道具为待删除状态
    auto item_aabb = item->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    createEffect(item_aabb.position + item_aabb.size / 2.0f, item->getTarget()); // 创建特效
    _context.getAudioPlayer().playSound("assets/audio/poka01.mp3");
}

void game::scene::GameScene::handleTileTriggers()
{
    const auto &tile_trigger_events = _context.getPhysicsEngine().getTileTriggerEvents();
    for (const auto &event : tile_trigger_events)
    {
        auto *obj = event.first;       // 触发事件的对象
        auto tile_type = event.second; // 瓦片类型
        if (tile_type == engine::component::TileType::HAZARD)
        {
            // 如果是玩家碰到了危险瓦片，就受伤
            if (obj->getName() == "player")
            {
                handlePlayerDamage(1);
            }
        }
    }
}

void game::scene::GameScene::handlePlayerDamage(int damage)
{
    auto player_component = _player->getComponent<game::component::PlayerComponent>();
    if (!player_component->takeDamage(damage))
    {
        return;
    }
    if (player_component->getDead())
    {
        spdlog::info("Game over");
    }
    _game_session_data->setCurrentHealth(player_component->getHealthComponent()->getCurrentHealth());
}

void game::scene::GameScene::createEffect(const glm::vec2 &center_pos, const std::string &tag)
{
    auto effect_obj = std::make_unique<engine::object::GameObject>("effect_" + tag);
    effect_obj->addComponent<engine::component::TransformComponent>(center_pos);

    // 根据标签创建不同的精灵组件和动画
    auto animation = std::make_unique<engine::render::Animation>("effect", false); // false表示不循环
    if (tag == "enemy")
    {
        effect_obj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/enemy-deadth.png",
                                                                     _context.getResourceManager(),
                                                                     engine::utils::Alignment::CENTER);
        for (auto i = 0; i < 5; ++i)
        {
            animation->addFrame({static_cast<int>(i * 40), 0, 40, 41}, 0.1f);
        }
    }
    else if (tag == "item")
    {
        effect_obj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/item-feedback.png", _context.getResourceManager(),
                                                                     engine::utils::Alignment::CENTER);
        for (auto i = 0; i < 4; ++i)
        {
            animation->addFrame({static_cast<int>(i * 32), 0, 32, 32}, 0.1f);
        }
    }
    else
    {
        return;
    }

    // 添加动画组件，并设置为单次播放后自动移除
    auto *animation_component = effect_obj->addComponent<engine::component::AnimationComponent>();
    animation_component->addAnimation(std::move(animation));
    animation_component->setOneShotRemoval(true); // 核心！
    animation_component->playAnimation("effect");

    safeAddGameObject(std::move(effect_obj)); // 安全添加特效对象
}

void game::scene::GameScene::toNextLevel(engine::object::GameObject *trigger)
{
    auto scene_name = trigger->getName();
    auto max_path = levelNameToPath(scene_name);
    _game_session_data->setNextLevel(max_path);
    auto next_scene = std::make_unique<game::scene::GameScene>(_context, _scene_manager, _game_session_data);
    _scene_manager.requestReplaceScene(std::move(next_scene));
}

void game::scene::GameScene::testTextRenderer()
{
    auto &text_renderer = _context.getTextRenderer();
    const auto &camera = _context.getCamera();
    text_renderer.drawUIText("UI Text", "assets/fonts/VonwaonBitmap-16px.ttf", 32, glm::vec2(100.0f), {0.0f, 1.0f, 0.0f, 1.0f});
    text_renderer.drawText(camera, "World Text", "assets/fonts/VonwaonBitmap-16px.ttf", 32, glm::vec2(200.0f));
}
