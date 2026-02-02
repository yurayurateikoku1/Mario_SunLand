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
    class HealthComponent;
    class AudioComponent;
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
        engine::component::HealthComponent *_health_component = nullptr;
        engine::component::AudioComponent *_audio_component = nullptr;

        std::unique_ptr<state::PlayerState> _current_state = nullptr;
        bool _is_dead = false;

        /// @brief 水平移动力
        float _move_force = 200.0f;
        /// @brief 最大移动速度
        float _move_speed = 120.0f;
        /// @brief 跳跃力
        float _jump_vel = 350.0f;
        /// @brief 摩擦系数
        float _friction_factor = 0.85f;

        float _stunned_duration = 0.4f;

        float _climb_speed = 100.0f;

        // 土狼时间
        static constexpr float _coyote_time = 0.2f;
        float _coyote_timer = 0.0f;
        static constexpr float _flash_interval = 0.1f;
        float _flash_timer = 0.0f;

    public:
        PlayerComponent();
        ~PlayerComponent() override;
        PlayerComponent(const PlayerComponent &) = delete;
        PlayerComponent(PlayerComponent &&) = delete;
        PlayerComponent &operator=(const PlayerComponent &) = delete;
        PlayerComponent &operator=(PlayerComponent &&) = delete;

        bool takeDamage(int damage_amount);

        engine::component::TransformComponent *getTrasformComponent() const { return _transform_component; }
        engine::component::PhysicsComponent *getPhysicsComponent() const { return _physics_component; }
        engine::component::SpriteComponent *getSpriteComponent() const { return _sprite_component; }
        engine::component::AnimationComponent *getAnimationComponent() const { return _animation_component; }
        engine::component::HealthComponent *getHealthComponent() const { return _health_component; }
        engine::component::AudioComponent *getAudioComponent() const { return _audio_component; }

        bool getDead() const { return _is_dead; }
        void setDead(bool is_dead) { _is_dead = is_dead; }
        void setMoveForce(float force) { _move_force = force; }
        float getMoveForce() const { return _move_force; }
        void setMoveSpeed(float speed) { _move_speed = speed; }
        float getMoveSpeed() const { return _move_speed; }
        void setJumpVelocity(float force) { _jump_vel = force; }
        float getJumpVelocity() const { return _jump_vel; }
        void setFrictionFactor(float factor) { _friction_factor = factor; }
        float getFrictionFactor() const { return _friction_factor; }
        void setStunnedDuration(float duration) { _stunned_duration = duration; }
        float getStunnedDuration() const { return _stunned_duration; }
        void setClimbSpeed(float speed) { _climb_speed = speed; }
        float getClimbSpeed() const { return _climb_speed; }

        void setState(std::unique_ptr<state::PlayerState> new_state);
        bool isOnGround() const;

    private:
        void init() override;
        void handleInput(engine::core::Context &context) override;
        void update(float dt, engine::core::Context &context) override;
    };
}