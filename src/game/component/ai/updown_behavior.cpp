#include "updown_behavior.h"
#include "../ai_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/animation_component.h"
#include <spdlog/spdlog.h>

game::component::ai::UpDownBehavior::UpDownBehavior(float min_y, float max_y, float speed)
    : _patrol_min_y(min_y), _patrol_max_y(max_y), _move_speed(speed)
{
    if (_patrol_min_y > _patrol_max_y)
    {
        _patrol_min_y = _patrol_max_y;
    }
}

void game::component::ai::UpDownBehavior::enter(AIComponent &ai_component)
{
    if (auto *ac = ai_component.getAnimationComponent())
    {
        ac->playAnimation("fly");
    }
    // 关键：禁用重力
    if (auto *pc = ai_component.getPhysicsComponent())
    {
        pc->setUseGravity(false);
    }
}

void game::component::ai::UpDownBehavior::update(float delta_time, AIComponent &ai_component)
{
    auto *pc = ai_component.getPhysicsComponent();
    auto *tc = ai_component.getTransformComponent();
    // ...
    auto current_y = tc->getPosition().y;

    // 到达上边界或碰到上方障碍，向下移动
    if (pc->getCollidedAbove() || current_y <= _patrol_min_y)
    {
        pc->_velocity.y = _move_speed;
        _moving_down = true;
    }
    // 到达下边界或碰到下方障碍，向上移动
    else if (pc->getCollidedAbove() || current_y >= _patrol_max_y)
    {
        pc->_velocity.y = -_move_speed;
        _moving_down = false;
    }
}
