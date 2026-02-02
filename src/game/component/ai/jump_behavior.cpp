#include "jump_behavior.h"
#include "../ai_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/audio_component.h"
game::component::ai::JumpBehavior::JumpBehavior(float min_x, float max_x, glm::vec2 jump_vel, float interval)
    : _patrol_min_x(min_x), _patrol_max_x(max_x), _jump_vel(jump_vel), _jump_interval(interval)
{
}

void game::component::ai::JumpBehavior::enter(AIComponent &ai_component)
{
}

void game::component::ai::JumpBehavior::update(float delta_time, AIComponent &ai_component)
{
    auto *pc = ai_component.getPhysicsComponent();
    auto *tc = ai_component.getTransformComponent();
    auto *sc = ai_component.getSpriteComponent();
    auto *ac = ai_component.getAudioComponent();
    if (!pc || !tc || !sc)
    {
        return;
    }

    auto is_on_ground = pc->getCollidedBelow();
    if (is_on_ground)
    {
        if (ac && _jump_timer < 0.001f)
        {
            ac->playerSound("cry", -1, true);
        }
        // 如果在地面上
        _jump_timer += delta_time; // 累加等待计时器
        pc->_velocity.x = 0.0f;    // 停止水平滑动

        if (_jump_timer >= _jump_interval)
        {                       // 等待时间到，准备跳跃
            _jump_timer = 0.0f; // 重置计时器

            // 检查是否需要转向
            auto current_x = ai_component.getTransformComponent()->getPosition().x;
            if (_jumping_right && (pc->getCollidedRight() || current_x >= _patrol_max_x))
            {
                _jumping_right = false;
            }
            else if (!_jumping_right && (pc->getCollidedLeft() || current_x <= _patrol_min_x))
            {
                _jumping_right = true;
            }

            // 设置跳跃速度并发起跳跃
            auto jump_vel_x = _jumping_right ? _jump_vel.x : -_jump_vel.x;
            pc->_velocity = {jump_vel_x, _jump_vel.y};
            ai_component.getAnimationComponent()->playAnimation("jump");
        }
        else
        { // 还在地面等待
            ai_component.getAnimationComponent()->playAnimation("idle");
        }
    }
    else
    { // 在空中
        // 根据垂直速度判断是上升(jump)还是下落(fall)来播放动画
        if (pc->getVelocity().y < 0)
        {
            ai_component.getAnimationComponent()->playAnimation("jump");
        }
        else
        {
            ai_component.getAnimationComponent()->playAnimation("fall");
        }
    }
}
