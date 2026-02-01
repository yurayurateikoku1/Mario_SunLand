#include "animation.h"
#include <glm/common.hpp>
#include <spdlog/spdlog.h>

engine::render::Animation::Animation(const std::string &name, bool loop)
    : _name(name), _loop(loop)
{
}

void engine::render::Animation::addFrame(const SDL_Rect &source_rect, float duration)
{
    if (duration <= 0.0f)
    {
        spdlog::warn("Animation frame duration must be greater than 0.0f", _name);
        return;
    }
    _frames.push_back({source_rect, duration});
    _total_duration += duration;
}

const engine::render::AnimationFrame &engine::render::Animation::getFrame(float time) const
{
    if (_frames.empty())
    {
        spdlog::error("Animation is empty: {}", _name);
        return _frames.back();
    }
    float current_time = time;
    if (_loop && _total_duration > 0.0f)
    {
        current_time = glm::mod(time, _total_duration);
    }
    else
    {
        if (current_time >= _total_duration)
        {
            return _frames.back();
        }
    }

    float accumulated_time = 0.0f;
    for (const auto &frame : _frames)
    {
        accumulated_time += frame.duration;
        if (current_time < accumulated_time)
        {
            return frame;
        }
    }
    return _frames.back();
}
