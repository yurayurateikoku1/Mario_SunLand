#include "animation_component.h"
#include "sprite_component.h"
#include "../object/game_object.h"
#include "../render/animation.h"

engine::component::AnimationComponent::~AnimationComponent() = default;

void engine::component::AnimationComponent::init()
{
    if (!_owner)
    {
        spdlog::error("AnimationComponent not has owner");
        return;
    }
    _sprite_component = _owner->getComponent<engine::component::SpriteComponent>();
    if (!_sprite_component)
    {
        spdlog::error("AnimationComponent not has sprite component");
        return;
    }
}

void engine::component::AnimationComponent::addAnimation(std::unique_ptr<engine::render::Animation> &&animation)
{
    if (!animation)
        return;
    std::string name = animation->getName();
    _animations[name] = std::move(animation);
}

void engine::component::AnimationComponent::playAnimation(const std::string &name)
{
    auto it = _animations.find(name);
    if (it == _animations.end())
    { /* ... 错误处理 ... */
        return;
    }

    // 如果已经在播放相同的动画，则不重新开始
    if (_current_animation == it->second.get() && _is_playing)
    {
        return;
    }

    _current_animation = it->second.get();
    _animation_timer = 0.0f; // 重置计时器
    _is_playing = true;

    // 立即将精灵更新到第一帧
    if (_sprite_component && !_current_animation->isEmpty())
    {
        const auto &first_frame = _current_animation->getFrame(0.0f);
        _sprite_component->setSourceRect(first_frame.source_rect);
    }
}

std::string engine::component::AnimationComponent::getCurrentAnimationName() const
{
    if (_current_animation)
    {
        return _current_animation->getName();
    }

    return std::string();
}

bool engine::component::AnimationComponent::isAnimationFinished() const
{
    if (!_current_animation || _current_animation->isLooping())
    {
        return false;
    }
    return _animation_timer >= _current_animation->getTotalDuration();
}

void engine::component::AnimationComponent::update(float delta_time, engine::core::Context &)
{
    if (!_is_playing || !_current_animation || !_sprite_component || _current_animation->isEmpty())
    {
        return;
    }

    // 推进计时器
    _animation_timer += delta_time;

    // 根据时间获取当前帧
    const auto &current_frame = _current_animation->getFrame(_animation_timer);

    // 更新精灵组件的源矩形
    _sprite_component->setSourceRect(current_frame.source_rect);
    if (!_current_animation->isLooping() && _animation_timer >= _current_animation->getTotalDuration())
    {
        _is_playing = false;
        _animation_timer = _current_animation->getTotalDuration();
        if (_is_one_shot_removal)
        {
            _owner->setNeedRemove(true);
        }
    }
}
