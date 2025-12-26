#include "time.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_timer.h>
engine::core::Time::Time()
{
    _last_time = SDL_GetTicksNS();
    _frame_start_time = _last_time;
    spdlog::trace("Time init ...,LastTime:{},FrameStartTime:{}", _last_time, _frame_start_time);
}

void engine::core::Time::update()
{
    _frame_start_time = SDL_GetTicksNS(); // 记录进入 update 时的时间戳
    auto current_delta_time = static_cast<double>(_frame_start_time - _last_time) / 1000000000.0f;
    if (_target_frame_time > 0)
    {
        limitFrameRate(current_delta_time);
    }
    else
    {
        _delta_time = current_delta_time;
    }
    _last_time = SDL_GetTicksNS(); // 记录离开 update 时的时间戳
}

void engine::core::Time::limitFrameRate(float current_delta_time)
{
    if (current_delta_time < _target_frame_time)
    {
        double time_to_wait = _target_frame_time - current_delta_time;
        Uint64 ns_to_wait = static_cast<Uint64>(time_to_wait * 1000000000.0f);
        SDL_DelayNS(ns_to_wait);
        _delta_time = static_cast<double>(SDL_GetTicksNS() - _last_time) / 1000000000.0f;
    }
}

float engine::core::Time::getDeltaTime() const
{
    return _delta_time * _time_scale;
}

float engine::core::Time::getUnScaledDeltaTime() const
{
    return _delta_time;
}

void engine::core::Time::setTimeScale(float scale)
{
    if (scale < 0)
    {
        /* code */
        spdlog::warn("Time scale must be greater than 0");
        scale = 0.0f;
    }
    _time_scale = scale;
}

void engine::core::Time::setTargetFps(int fps)
{
    if (fps < 0)
    {
        spdlog::warn("Target FPS must be greater than 0");
        fps = 0;
    }
    else
    {
        _target_fps = fps;
    }

    if (_target_fps > 0)
    {
        _target_frame_time = 1.0f / static_cast<double>(_target_fps);
        spdlog::info("Target FPS set to {}", _target_fps);
    }
    else
    {
        _target_frame_time = 0.0f;
        spdlog::info("Target FPS set Unlimited");
    }
}
