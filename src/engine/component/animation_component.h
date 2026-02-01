#pragma once
#include "component.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace engine::render
{
    class Animation;
}

namespace engine::component
{
    class SpriteComponent;
}

namespace engine::component
{
    class AnimationComponent : public Component
    {
        friend class engine::object::GameObject;

    private:
        std::unordered_map<std::string, std::unique_ptr<engine::render::Animation>> _animations;
        SpriteComponent *_sprite_component = nullptr;
        engine::render::Animation *_current_animation = nullptr;

        float _animation_timer = 0.0f;
        bool _is_playing = false;
        bool _is_one_shot_removal = false;

    public:
        AnimationComponent() = default;
        ~AnimationComponent() override;
        AnimationComponent(const AnimationComponent &) = delete;
        AnimationComponent(AnimationComponent &&) = delete;
        AnimationComponent &operator=(const AnimationComponent &) = delete;
        AnimationComponent &operator=(AnimationComponent &&) = delete;

        void addAnimation(std::unique_ptr<engine::render::Animation> &&animation);
        void playAnimation(const std::string &name);
        void stopAnimation() { _is_playing = false; }

        std::string getCurrentAnimationName() const;
        bool isPlaying() const { return _is_playing; }
        bool isAnimationFinished() const;
        bool isOneShotRemoval() const { return _is_one_shot_removal; }
        void setOneShotRemoval(bool value) { _is_one_shot_removal = value; }

    protected:
        void init() override;
        void update(float dt, engine::core::Context &) override;
    };
}