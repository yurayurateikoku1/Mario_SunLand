#pragma once
#include "../../engine/component/component.h"
#include "ai/ai_behavior.h"
#include <memory>
namespace engine::component
{
    class AIBehavior;
}
namespace engine::component
{
    class TransformComponent;
    class PhysicsComponent;
    class SpriteComponent;
    class AnimationComponent;
    class AudioComponent;
}

namespace game::component
{

    /**
     * @brief 负责管理 GameObject 的 AI 行为。
     */
    class AIComponent final : public engine::component::Component
    {
        friend class engine::object::GameObject;

    private:
        std::unique_ptr<ai::AIBehavior> _current_behavior = nullptr; ///< @brief 当前 AI 行为策略

        // --- 缓存组件指针 ---
        engine::component::TransformComponent *_transform_component = nullptr;
        engine::component::PhysicsComponent *_physics_component = nullptr;
        engine::component::SpriteComponent *_sprite_component = nullptr;
        engine::component::AnimationComponent *_animation_component = nullptr;
        engine::component::AudioComponent *_audio_component = nullptr;

    public:
        void setBehavior(std::unique_ptr<ai::AIBehavior> behavior);
        bool takeDamage(int damage);
        bool isAlive() const;
        // --- Getters for behaviors ---
        engine::component::PhysicsComponent *getPhysicsComponent() const { return _physics_component; }
        engine::component::TransformComponent *getTransformComponent() const { return _transform_component; }
        engine::component::SpriteComponent *getSpriteComponent() const { return _sprite_component; }
        engine::component::AnimationComponent *getAnimationComponent() const { return _animation_component; }
        engine::component::AudioComponent *getAudioComponent() const { return _audio_component; }

    private:
        void init() override;
        void update(float delta_time, engine::core::Context &) override;
    };

} // namespace game::component