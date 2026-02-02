#include "climb_state.h"
#include "jump_state.h"
#include "fall_state.h"
#include "../player_component.h"
#include "../../../engine/physics/physics_engine.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/object/game_object.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/collider_component.h"
#include "../../../engine/component/animation_component.h"
void game::component::state::ClimbState::enter()
{
    playerAnimation("climb");
    if (auto *physics = _player_component->getPhysicsComponent(); physics)
    {
        physics->setUseGravity(false);
    }
}

void game::component::state::ClimbState::exit()
{
    if (auto *physics = _player_component->getPhysicsComponent(); physics)
    {
        physics->setUseGravity(true);
    }
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::ClimbState::handleInput(engine::core::Context &context)
{
    auto input_manager = context.getInputManager();
    auto physics_component = _player_component->getPhysicsComponent();
    auto animation_component = _player_component->getAnimationComponent();

    auto is_up = input_manager.isActionDown("move_up");
    auto is_down = input_manager.isActionDown("move_down");
    auto is_left = input_manager.isActionDown("move_left");
    auto is_right = input_manager.isActionDown("move_right");
    auto speed = _player_component->getClimbSpeed();

    physics_component->_velocity.y = is_up ? -speed : is_down ? speed
                                                              : 0.0f;
    physics_component->_velocity.x = is_left ? -speed : is_right ? speed
                                                                 : 0.0f;

    (is_up || is_down || is_left || is_right) ? animation_component->resumeAnimation() : animation_component->stopAnimation();

    if (input_manager.isActionPressed("jump"))
    {
        return std::make_unique<JumpState>(_player_component);
    }

    return nullptr;
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::ClimbState::update(float dt, engine::core::Context &)
{
    auto physics_component = _player_component->getPhysicsComponent();
    if (physics_component->getCollidedBelow())
    {
        return std::make_unique<JumpState>(_player_component);
    }
    if (!physics_component->getCollidedLoadder())
    {
        return std::make_unique<FallState>(_player_component);
    }
    return nullptr;
}
