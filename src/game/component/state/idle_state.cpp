#include "idle_state.h"
#include "jump_state.h"
#include "fall_state.h"
#include "walk_state.h"
#include "climb_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/object/game_object.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../player_component.h"
void game::component::state::IdleState::enter()
{
    playerAnimation("idle");
}

void game::component::state::IdleState::exit()
{
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::IdleState::handleInput(engine::core::Context &context)
{
    auto input_manager = context.getInputManager();
    auto physics_component = _player_component->getPhysicsComponent();
    if (physics_component->getCollidedLoadder() && input_manager.isActionDown("move_up"))
    {
        return std::make_unique<ClimbState>(_player_component);
    }

    if (physics_component->isOnTopLadder() && input_manager.isActionDown("move_down"))
    {
        _player_component->getTrasformComponent()->translate(glm::vec2(0, 2.0f));
        return std::make_unique<ClimbState>(_player_component);
    }

    // 如果按下了左右移动,切换到行走状态
    if (input_manager.isActionDown("move_left") || input_manager.isActionDown("move_right"))
    {
        return std::make_unique<WalkState>(_player_component);
    }

    // 如果按下了跳跃键,切换到跳跃状态
    if (input_manager.isActionPressed("jump"))
    {
        return std::make_unique<JumpState>(_player_component);
    }
    return nullptr;
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::IdleState::update(float dt, engine::core::Context &)
{
    auto physics_component = _player_component->getPhysicsComponent();
    auto friction_factor = _player_component->getFrictionFactor();

    physics_component->_velocity.x *= friction_factor;
    if (!physics_component->getCollidedBelow())
    {
        return std::make_unique<FallState>(_player_component);
    }

    return nullptr;
}
