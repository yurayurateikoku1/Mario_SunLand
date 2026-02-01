#include "fall_state.h"
#include "jump_state.h"
#include "idle_state.h"
#include "walk_state.h"
#include "../player_component.h"
#include "../../../engine/physics/physics_engine.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/object/game_object.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include <glm/common.hpp>

void game::component::state::FallState::enter()
{
    playerAnimation("fall");
}

void game::component::state::FallState::exit()
{
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::FallState::handleInput(engine::core::Context &context)
{
    auto input_manager = context.getInputManager();
    auto physics_component = _player_component->getPhysicsComponent();
    auto sprite_component = _player_component->getSpriteComponent();

    if (input_manager.isActionDown("move_left"))
    {
        if (physics_component->_velocity.x > 0.0f)
        {
            physics_component->_velocity.x = 0.0f;
        }
        physics_component->addForce({-_player_component->getMoveForce(), 0.0f});
        sprite_component->setFlipped(true);
    }
    else if (input_manager.isActionDown("move_right"))
    {
        if (physics_component->_velocity.x < 0.0f)
        {
            physics_component->_velocity.x = 0.0f;
        }
        physics_component->addForce({_player_component->getMoveForce(), 0.0f});
        sprite_component->setFlipped(false);
    }
    return nullptr;
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::FallState::update(float dt, engine::core::Context &)
{
    auto physics_component = _player_component->getPhysicsComponent();
    auto max_speed = _player_component->getMoveSpeed();

    physics_component->_velocity.x = glm::clamp(physics_component->_velocity.x, -max_speed, max_speed);

    if (physics_component->getCollidedBelow())
    {
        if (glm::abs(physics_component->_velocity.x) < 1.0f)
        {
            return std::make_unique<IdleState>(_player_component);
        }
        else
        {
            return std::make_unique<WalkState>(_player_component);
        }
    }
    return std::unique_ptr<PlayerState>();
}
