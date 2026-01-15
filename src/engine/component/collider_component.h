#pragma once
#include "./component.h"
#include "../physics/collider.h"
#include "../utils/math.h"
#include "../utils/alignment.h"
#include <memory>

namespace engine::component
{
    class TransformComponent;
}

namespace engine::component
{
    /// @brief 碰撞盒组件
    class ColliderComponent final : public Component
    {
        friend class engine::object::GameObject;

    private:
        TransformComponent *_transform = nullptr;
        /// @brief 碰撞器对象
        std::unique_ptr<engine::physics::Collider> _collider;
        glm::vec2 _offset{0.0f, 0.0f};
        engine::utils::Alignment _alignment = engine::utils::Alignment::NONE;

        /// @brief 是否为触发器
        bool _is_trigger = false;
        /// @brief 是否激活
        bool _is_active = true;

    public:
        explicit ColliderComponent(std::unique_ptr<engine::physics::Collider> collider,
                                   engine::utils::Alignment alignment = engine::utils::Alignment::NONE,
                                   bool is_trigger = false,
                                   bool is_active = true);

        /// @brief 更新碰撞盒偏移,根据_alignment_
        void updateOffset();

        TransformComponent *getTransform() const { return _transform; }
        const engine::physics::Collider *getCollider() const { return _collider.get(); }
        const glm::vec2 &getOffset() const { return _offset; }
        engine::utils::Alignment getAlignment() const { return _alignment; }
        engine::utils::Rect getWorldAABB() const;
        bool isTrigger() const { return _is_trigger; }
        bool isActive() const { return _is_active; }

        void setAlignment(engine::utils::Alignment alignment);
        void setOffset(const glm::vec2 &offset) { _offset = offset; }
        void setTrigger(bool is_trigger) { _is_trigger = is_trigger; }
        void setActive(bool is_active) { _is_active = is_active; }

    private:
        void init() override;
        void update(float, engine::core::Context &) override {}
    };
}