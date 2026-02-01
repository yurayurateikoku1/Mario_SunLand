#pragma once
#include "../../engine/component/component.h"
#include <memory>

namespace engine::input
{
    class InputManager;
}

namespace engine::component
{
    class TransformComponent;
    class PhysicsComponent;
    class SpriteComponent;
    class AnimationComponent;
}

namespace game::component::state
{
    class PlayerState;
}

namespace game::component
{
    class PlayerComponent final : public engine::component::Component
    {
        friend class engine::object::GameObject;

    private:
        engine::component::TransformComponent *_transform_component = nullptr;
        engine::component::PhysicsComponent *_physics_component = nullptr;
        engine::component::SpriteComponent *_sprite_component = nullptr;
        engine::component::AnimationComponent *_animation_component = nullptr;

        std::unique_ptr<state::PlayerState> _current_state = nullptr;
        bool _is_dead = false;

        /// @brief 水平移动力
        float _move_force = 200.0f;
        /// @brief 最大移动速度
        float _move_speed = 120.0f;
        /// @brief 跳跃力
        float _jump_force = 350.0f;
        /// @brief 摩擦系数
        float _friction_factor = 0.85f;

    public:
        PlayerComponent();
        ~PlayerComponent() override;
        PlayerComponent(const PlayerComponent &) = delete;
        PlayerComponent(PlayerComponent &&) = delete;
        PlayerComponent &operator=(const PlayerComponent &) = delete;
        PlayerComponent &operator=(PlayerComponent &&) = delete;

        engine::component::TransformComponent *getTrasformComponent() const { return _transform_component; }
        engine::component::PhysicsComponent *getPhysicsComponent() const { return _physics_component; }
        engine::component::SpriteComponent *getSpriteComponent() const { return _sprite_component; }
        engine::component::AnimationComponent *getAnimationComponent() const { return _animation_component; }

        bool getDead() const { return _is_dead; }
        void setDead(bool is_dead) { _is_dead = is_dead; }
        void setMoveForce(float force) { _move_force = force; }
        float getMoveForce() const { return _move_force; }
        void setMoveSpeed(float speed) { _move_speed = speed; }
        float getMoveSpeed() const { return _move_speed; }
        void setJumpForce(float force) { _jump_force = force; }
        float getJumpForce() const { return _jump_force; }
        void setFrictionFactor(float factor) { _friction_factor = factor; }
        float getFrictionFactor() const { return _friction_factor; }

        void setState(std::unique_ptr<state::PlayerState> new_state);

    private:
        void init() override;
        void handleInput(engine::core::Context &context) override;
        void update(float dt, engine::core::Context &context) override;
    };
}