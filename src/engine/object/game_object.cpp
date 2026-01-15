#include "game_object.h"
#include <spdlog/spdlog.h>
#include "../render/render.h"
#include "../input/input_manager.h"
#include "../render/camera.h"

engine::object::GameObject::GameObject(const std::string &name, const std::string &tag)
    : _name(name), _target(tag)
{
    spdlog::info("GameObject {} created", name);
}

void engine::object::GameObject::update(float dt, engine::core::Context &context)
{
    for (auto &pair : _components)
    {
        /* code */
        pair.second->update(dt, context);
    }
}

void engine::object::GameObject::render(engine::core::Context &context)
{
    for (auto &pair : _components)
    {
        /* code */
        pair.second->render(context);
    }
}

void engine::object::GameObject::clean()
{
    for (auto &pair : _components)
    {
        /* code */
        pair.second->clean();
    }
    _components.clear();
}

void engine::object::GameObject::handleInput(engine::core::Context &context)
{
    for (auto &pair : _components)
    {
        /* code */
        pair.second->handleInput(context);
    }
}
