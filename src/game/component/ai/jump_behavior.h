#pragma once
#include <glm/vec2.hpp>
#include "ai_behavior.h"
namespace game::component::ai
{
    class JumpBehavior : public AIBehavior
    {
        friend class game::component::AIComponent;

    private:
        float _patrol_min_x;                              ///< @brief 巡逻范围最小X
        float _patrol_max_x;                              ///< @brief 巡逻范围最大X
        glm::vec2 _jump_vel = glm::vec2(100.0f, -300.0f); ///< @brief 跳跃速度（x, y）
        float _jump_interval = 2.0f;                      ///< @brief 跳跃间隔时间

        float _jump_timer = 0.0f;    ///< @brief 跳跃计时器
        bool _jumping_right = false; ///< @brief 是否向右跳
    public:
        JumpBehavior(float min_x, float max_x,
                     glm::vec2 jump_vel = {120.0f, -300.0f},
                     float interval = 2.0f);

    protected:
        void enter(AIComponent &ai_component) override;
        void update(float delta_time, AIComponent &ai_component) override;
    };
}