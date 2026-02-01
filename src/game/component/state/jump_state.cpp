#include "jump_state.h"
#include "fall_state.h"
#include "../player_component.h"
#include "../../../engine/physics/physics_engine.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/object/game_object.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
void game::component::state::JumpState::enter()
{
    playerAnimation("jump");
    auto physics_component = _player_component->getPhysicsComponent();
    physics_component->_velocity.y = -_player_component->getJumpForce();
}

void game::component::state::JumpState::exit()
{
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::JumpState::handleInput(engine::core::Context &context)
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
        /* code */
        if (physics_component->_velocity.x < 0.0f)
        {
            physics_component->_velocity.x = 0.0f;
        }
        physics_component->addForce({_player_component->getMoveForce(), 0.0f});
        sprite_component->setFlipped(false);
    }

    return nullptr;
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::JumpState::update(float dt, engine::core::Context &)
{
    auto physics_component = _player_component->getPhysicsComponent();
    auto max_speed = _player_component->getMoveSpeed();
    physics_component->_velocity.x = glm::clamp(physics_component->_velocity.x, -max_speed, max_speed);
    if (physics_component->_velocity.y > 0.0f)
    {
        return std::make_unique<FallState>(_player_component);
    }
    return nullptr;
}
