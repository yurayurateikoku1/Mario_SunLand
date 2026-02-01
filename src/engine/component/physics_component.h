#pragma once
#include "component.h"
#include <glm/vec2.hpp>
namespace engine::physics
{
    class PhysicsEngine;
}

namespace engine::component
{
    class TransformComponent;

    /// @brief 物理组件
    class PhysicsComponent final : public Component
    {
        friend class engine::object::GameObject;

    public:
        glm::vec2 _velocity{0.0f, 0.0f}; // 移动速度

    private:
        engine::physics::PhysicsEngine *_physics_engine = nullptr;
        TransformComponent *_transform = nullptr;

        /// @brief 当前帧受力
        glm::vec2 _force = {0.0f, 0.0f};
        /// @brief 物体质量
        float _mass = 1.0f;
        /// @brief 物理是否受重力影响
        bool _use_gravity = true;
        /// @brief 主件是否启用
        bool _enabled = true;

        /// 碰撞状态标志
        bool _collided_below = false;
        bool _collided_above = false;
        bool _collided_left = false;
        bool _collided_right = false;

    public:
        PhysicsComponent(engine::physics::PhysicsEngine *physics_engine, bool use_gravity = true, float mass = 1.0f);
        ~PhysicsComponent() override = default;
        PhysicsComponent(const PhysicsComponent &) = delete;
        PhysicsComponent(PhysicsComponent &&) = delete;
        PhysicsComponent &operator=(const PhysicsComponent &) = delete;
        PhysicsComponent &operator=(PhysicsComponent &&) = delete;

        void addForce(const glm::vec2 &force)
        {
            if (_enabled)
                _force += force;
        }
        void clearForce() { _force = {0.0f, 0.0f}; }
        const glm::vec2 &getForce() const { return _force; }

        float getMass() const { return _mass; }
        void setMass(float mass) { _mass = (mass > 0.0f) ? mass : 1.0f; }
        bool getUseGravity() const { return _use_gravity; }
        void setUseGravity(bool use_gravity) { _use_gravity = use_gravity; }
        bool getEnabled() const { return _enabled; }
        void setEnabled(bool enabled) { _enabled = enabled; }
        void setVelocity(const glm::vec2 &velocity) { _velocity = velocity; }
        TransformComponent *getTransform() const { return _transform; }

        void resetCollidedFlags()
        {
            _collided_below = false;
            _collided_above = false;
            _collided_left = false;
            _collided_right = false;
        }

        void setCollidedBelow(bool collided) { _collided_below = collided; }
        void setCollidedAbove(bool collided) { _collided_above = collided; }
        void setCollidedLeft(bool collided) { _collided_left = collided; }
        void setCollidedRight(bool collided) { _collided_right = collided; }
        void setVelocity(glm::vec2 velocity) { _velocity = std::move(velocity); }
        const glm::vec2 &getVelocity() const { return _velocity; }
        bool getCollidedBelow() const { return _collided_below; }
        bool getCollidedAbove() const { return _collided_above; }
        bool getCollidedLeft() const { return _collided_left; }
        bool getCollidedRight() const { return _collided_right; }

    private:
        void init() override;
        void update(float dt, engine::core::Context &) override {}
        void clean() override;
    };
}
