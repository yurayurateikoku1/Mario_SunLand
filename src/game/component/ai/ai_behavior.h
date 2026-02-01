#pragma once

namespace game::component
{
    class AIComponent;
}

namespace game::component::ai
{

    /**
     * @brief AI 行为策略的抽象基类。
     */
    class AIBehavior
    {
        friend class game::component::AIComponent;

    public:
        AIBehavior() = default;
        virtual ~AIBehavior() = default;
        AIBehavior(const AIBehavior &) = delete;
        AIBehavior(AIBehavior &&) = delete;
        AIBehavior &operator=(const AIBehavior &) = delete;
        AIBehavior &operator=(AIBehavior &&) = delete;

    protected:
        virtual void enter(AIComponent &) {}           ///< @brief enter函数可选是否实现，默认为空
        virtual void update(float, AIComponent &) = 0; ///< @brief 更新 AI 行为逻辑(具体策略)，必须实现
    };

} // namespace game::component::ai