#pragma once
#include "ai_behavior.h"
namespace game::component::ai
{

    class UpDownBehavior final : public AIBehavior
    {
        friend class game::component::AIComponent;

    private:
        float _patrol_min_y; ///< @brief 飞行范围最小Y
        float _patrol_max_y; ///< @brief 飞行范围最大Y
        float _move_speed;   ///< @brief 移动速度
        bool _moving_down = false;

    public:
        UpDownBehavior(float min_y, float max_y, float speed = 60.0f);
        ~UpDownBehavior() override = default;
        UpDownBehavior(const UpDownBehavior &) = delete;
        UpDownBehavior(UpDownBehavior &&) = delete;
        UpDownBehavior &operator=(const UpDownBehavior &) = delete;
        UpDownBehavior &operator=(UpDownBehavior &&) = delete;

    protected:
        void enter(AIComponent &ai_component) override;
        void update(float delta_time, AIComponent &ai_component) override;
    };

} // namespace game::component::ai