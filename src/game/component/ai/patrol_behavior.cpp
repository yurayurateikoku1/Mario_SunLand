#include "patrol_behavior.h"
#include "../ai_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/sprite_component.h"
game::component::ai::PatrolBehavior::PatrolBehavior(float min_x, float max_x, float speed)
    : _patrol_min_x(min_x), _patrol_max_x(max_x), _move_speed(speed)
{
    if (_patrol_min_x > _patrol_max_x)
    {
        _patrol_min_x = _patrol_max_x;
    }
}

void game::component::ai::PatrolBehavior::enter(AIComponent &ai_component)
{
    if (auto *ac = ai_component.getAnimationComponent(); ac)
    {
        ac->playAnimation("walk");
    }
}

void game::component::ai::PatrolBehavior::update(float delta_time, AIComponent &ai_component)
{
    auto *pc = ai_component.getPhysicsComponent();
    auto *tc = ai_component.getTransformComponent();
    auto *sc = ai_component.getSpriteComponent();
    if (!pc || !tc || !sc)
    {
        return;
    }

    auto current_x = tc->getPosition().x;

    // 撞右墙或到达右边界则转向左
    if (pc->getCollidedRight() || current_x >= _patrol_max_x)
    {
        pc->_velocity.x = -_move_speed;
        _moving_right = false;
        // 撞左墙或到达左边界则转向右
    }
    else if (pc->getCollidedLeft() || current_x <= _patrol_min_x)
    {
        pc->_velocity.x = _move_speed;
        _moving_right = true;
    }

    // 更新精灵翻转(向右移动时翻转)
    sc->setFlipped(_moving_right);
}
