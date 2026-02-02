#include "walk_state.h"
#include "jump_state.h"
#include "fall_state.h"
#include "idle_state.h"
#include "climb_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/object/game_object.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include <glm/common.hpp>
void game::component::state::WalkState::enter()
{
    playerAnimation("walk");
}

void game::component::state::WalkState::exit()
{
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::WalkState::handleInput(engine::core::Context &context)
{
    auto input_manager = context.getInputManager();
    auto physics_component = _player_component->getPhysicsComponent();
    auto sprite_component = _player_component->getSpriteComponent();

    if (physics_component->getCollidedLoadder() && input_manager.isActionDown("move_up"))
    {
        return std::make_unique<ClimbState>(_player_component);
    }

    // 如果按下“jump”键,切换到跳跃状态
    if (input_manager.isActionPressed("jump"))
    {
        return std::make_unique<JumpState>(_player_component);
    }

    // 左右移动
    if (input_manager.isActionDown("move_left"))
    {
        if (physics_component->_velocity.x > 0.0f)
        {
            physics_component->_velocity.x = 0.0f;
        }
        // 添加左移动力
        physics_component->addForce({-_player_component->getMoveForce(), 0.0f});
        sprite_component->setFlipped(true); // 翻转
    }
    else if (input_manager.isActionDown("move_right"))
    {
        if (physics_component->_velocity.x < 0.0f)
        {
            physics_component->_velocity.x = 0.0f;
        }
        // 添加右移动力
        physics_component->addForce({_player_component->getMoveForce(), 0.0f});
        sprite_component->setFlipped(false); // 不翻转
    }
    else
    {
        return std::make_unique<IdleState>(_player_component);
    }
    return nullptr;
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::WalkState::update(float dt, engine::core::Context &)
{
    // 限制最大速度
    auto physics_compont = _player_component->getPhysicsComponent();
    auto max_speed = _player_component->getMoveSpeed();
    physics_compont->_velocity.x = glm::clamp(physics_compont->_velocity.x, -max_speed, max_speed);

    // 如果下方没有碰撞 切换到下落状态
    if (!physics_compont->getCollidedBelow())
    {
        return std::make_unique<FallState>(_player_component);
    }
    return nullptr;
}
