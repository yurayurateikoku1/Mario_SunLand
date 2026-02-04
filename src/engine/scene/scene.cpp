#include "scene.h"
#include <spdlog/spdlog.h>
#include "scene_manager.h"
#include "../object/game_object.h"
#include "../render/camera.h"
#include "../core/context.h"
#include "../core/game_state.h"
#include "../ui/ui_manager.h"
#include "../physics/physics_engine.h"
#include <algorithm>
engine::scene::Scene::Scene(const std::string &scene_name, engine::core::Context &context, engine::scene::SceneManager &scene_manager)
    : _scene_name(scene_name), _context(context), _scene_manager(scene_manager), _is_initialized(false), _ui_manager(std::make_unique<engine::ui::UIManager>())
{
    spdlog::info("Scene {} created", _scene_name);
}

engine::scene::Scene::~Scene() = default;

void engine::scene::Scene::init()
{
    _is_initialized = true;
    spdlog::info("Scene {} initialized", _scene_name);
}

void engine::scene::Scene::update(float dt)
{
    if (!_is_initialized)
    {
        return;
    }
    if (_context.getGameState().isPlaying())
    {
        // 更新物理引擎
        _context.getPhysicsEngine().update(dt);
        _context.getCamera().update(dt);
    }

    // 更新所有游戏对象
    for (auto it = _game_objects.begin(); it != _game_objects.end();)
    {
        if (*it && !(*it)->getNeedRemove())
        {
            (*it)->update(dt, _context);
            ++it;
        }
        else
        {
            if (*it)
            {
                (*it)->clean();
            }
            it = _game_objects.erase(it);
        }
    }
    _ui_manager->update(dt, _context);
    processPendingAdditions();
}

void engine::scene::Scene::render()
{
    if (!_is_initialized)
    {
        /* code */
        return;
    }
    for (const auto &game_object : _game_objects)
    {
        if (game_object)
        {
            game_object->render(_context);
        }
    }
    _ui_manager->render(_context);
}

void engine::scene::Scene::handleInput()
{
    if (!_is_initialized)
    {
        return;
    }
    if (_ui_manager->handleInput(_context))
    {
        return;
    }

    for (auto it = _game_objects.begin(); it != _game_objects.end();)
    {
        if (*it && !(*it)->getNeedRemove())
        {
            (*it)->handleInput(_context);
            ++it;
        }
        else
        {
            if (*it)
            {
                (*it)->clean();
            }
            it = _game_objects.erase(it);
        }
    }
}

void engine::scene::Scene::clean()
{
    if (!_is_initialized)
    {
        /* code */
        return;
    }
    for (const auto &game_object : _game_objects)
    {
        if (game_object)
        {
            game_object->clean();
        }
    }
    _game_objects.clear();
    _is_initialized = false;
    spdlog::info("Scene {} cleaned", _scene_name);
}

void engine::scene::Scene::addGameObject(std::unique_ptr<engine::object::GameObject> &&game_object)
{
    if (game_object)
    {
        /* code */
        _game_objects.push_back(std::move(game_object));
    }
    else
        spdlog::warn("{} scene add game object is nullptr", _scene_name);
}

void engine::scene::Scene::safeAddGameObject(std::unique_ptr<engine::object::GameObject> &&game_object)
{
    if (game_object)
    {
        _pending_additions.push_back(std::move(game_object));
    }
    else
        spdlog::warn("{} scene add game object is nullptr", _scene_name);
}

void engine::scene::Scene::removeGameObject(engine::object::GameObject *game_object_ptr)
{
    if (!game_object_ptr)
    {
        /* code */
        spdlog::warn("{} scene remove game object is nullptr", _scene_name);
        return;
    }
    auto it = std::remove_if(_game_objects.begin(), _game_objects.end(),
                             [game_object_ptr](std::unique_ptr<engine::object::GameObject> &obj)
                             { return obj.get() == game_object_ptr; });
    if (it != _game_objects.end())
    {
        /* code */
        (*it)->clean();
        _game_objects.erase(it, _game_objects.end());
        spdlog::info("{} scene remove game object", _scene_name);
    }
    else
    {
        spdlog::warn("{} scene remove game object not found", _scene_name);
    }
}

void engine::scene::Scene::safeRemoveGameObject(engine::object::GameObject *game_object_ptr)
{
    game_object_ptr->setNeedRemove(true);
}

engine::object::GameObject *engine::scene::Scene::findGameObjectByName(const std::string &name) const
{
    for (const auto &game_object : _game_objects)
    {
        /* code */
        if (game_object && game_object->getName() == name)
        {
            /* code */
            return game_object.get();
        }
    }
    return nullptr;
}

void engine::scene::Scene::processPendingAdditions()
{
    for (auto &game_object : _pending_additions)
    {
        /* code */
        _game_objects.push_back(std::move(game_object));
    }
    _pending_additions.clear();
}
