#include "player_component.h"
#include "state/player_state.h"
#include "state/idle_state.h"
#include "state/hurt_state.h"
#include "state/dead_state.h"
#include <spdlog/spdlog.h>
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/input/input_manager.h"
#include <utility>
#include <typeinfo>

game::component::PlayerComponent::PlayerComponent() = default;

void game::component::PlayerComponent::init()
{
    if (!_owner)
    {
        spdlog::error("PlayerComponent: Owner is not set.");
        return;
    }

    _transform_component = _owner->getComponent<engine::component::TransformComponent>();
    _physics_component = _owner->getComponent<engine::component::PhysicsComponent>();
    _sprite_component = _owner->getComponent<engine::component::SpriteComponent>();
    _animation_component = _owner->getComponent<engine::component::AnimationComponent>();
    _health_component = _owner->getComponent<engine::component::HealthComponent>();

    if (!_transform_component || !_physics_component || !_sprite_component || !_animation_component || !_health_component)
    {
        spdlog::error("PlayerComponent: component is not set.");
        return;
    }
    _current_state = std::make_unique<game::component::state::IdleState>(this);
    if (_current_state)
    {
        setState(std::move(_current_state));
    }
    else
    {
        spdlog::error("PlayerComponent: current state is not set.");
    }
}

game::component::PlayerComponent::~PlayerComponent() = default;

bool game::component::PlayerComponent::takeDamage(int damage_amount)
{
    if (_is_dead || !_health_component || damage_amount <= 0)
    {
        return false;
    }

    bool success = _health_component->takeDamage(damage_amount);
    if (!success)
        return false;

    // --- 成功造成伤害了，根据是否存活决定状态切换
    if (_health_component->isAlive())
    {
        // 切换到受伤状态
        setState(std::make_unique<state::HurtState>(this));
    }
    else
    {
        _is_dead = true;
        // 切换到死亡状态
        setState(std::make_unique<state::DeadState>(this));
    }
    return true;
}

void game::component::PlayerComponent::setState(std::unique_ptr<game::component::state::PlayerState> new_state)
{
    if (!new_state)
    {
        spdlog::warn("new_state is nullptr");
        return;
    }
    if (_current_state)
    {
        _current_state->exit();
    }
    _current_state = std::move(new_state);
    _current_state->enter();
}

void game::component::PlayerComponent::handleInput(engine::core::Context &context)
{
    if (!_current_state)
    {
        return;
    }
    auto next_state = _current_state->handleInput(context);
    if (next_state)
    {
        setState(std::move(next_state));
    }
}

void game::component::PlayerComponent::update(float dt, engine::core::Context &context)
{
    if (!_current_state)
    {
        return;
    }
    auto next_state = _current_state->update(dt, context);
    if (next_state)
    {
        setState(std::move(next_state));
    }
}
