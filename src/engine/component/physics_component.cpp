#include "physics_component.h"
#include "transform_component.h"
#include "../object/game_object.h"
#include <spdlog/spdlog.h>
#include "../physics/physics_engine.h"

engine::component::PhysicsComponent::PhysicsComponent(engine::physics::PhysicsEngine *physics_engine, bool use_gravity, float mass)
    : _physics_engine(physics_engine), _use_gravity(use_gravity), _mass(mass >= 0.0f ? mass : 1.0f)
{
    if (!_physics_engine)
    {
        spdlog::error("Physics engine is null");
    }
    spdlog::info("Physics component created");
}

void engine::component::PhysicsComponent::init()
{
    if (!_owner)
    {
        spdlog::error("PhysicsComponent: Owner is not set.");
        return;
    }
    if (!_physics_engine)
    {
        spdlog::error("Physics engine is null");
        return;
    }
    _transform = _owner->getComponent<engine::component::TransformComponent>();
    if (!_transform)
    {
        spdlog::warn("physics component: game object {} does not have a TransformComponent.", _owner->getName());
        return;
    }
    _physics_engine->registerComponent(this);
    spdlog::info("Physics component initialized");
}

void engine::component::PhysicsComponent::clean()
{
    _physics_engine->unregisterComponent(this);
    spdlog::info("Physics component cleaned");
}
