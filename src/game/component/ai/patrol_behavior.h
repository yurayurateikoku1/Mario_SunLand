#pragma once
#include "ai_behavior.h"

namespace game::component::ai
{

    class PatrolBehavior final : public AIBehavior
    {
        friend class game::component::AIComponent;

    private:
        float _patrol_min_x;       ///< @brief 巡逻范围最小X
        float _patrol_max_x;       ///< @brief 巡逻范围最大X
        float _move_speed;         ///< @brief 移动速度
        bool _moving_right = true; ///< @brief 是否向右移动

    public:
        PatrolBehavior(float min_x, float max_x, float speed = 80.0f);
        ~PatrolBehavior() override = default;
        PatrolBehavior(const PatrolBehavior &) = delete;
        PatrolBehavior(PatrolBehavior &&) = delete;
        PatrolBehavior &operator=(const PatrolBehavior &) = delete;
        PatrolBehavior &operator=(PatrolBehavior &&) = delete;

    protected:
        void enter(AIComponent &ai_component) override;
        void update(float delta_time, AIComponent &ai_component) override;
    };

}