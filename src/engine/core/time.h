#pragma once
#include <SDL3/SDL_stdinc.h>

namespace engine::core
{

    class Time final
    {
    private:
        /// @brief 上一帧的时间
        Uint64 _last_time{0};
        /// @brief 当前帧开始的时间戳
        Uint64 _frame_start_time{0};
        /// @brief 帧的时间差
        double _delta_time{0.0f};
        /// @brief 时间缩放因子
        double _time_scale{1.0f};

        /// @brief 目标帧率
        int _target_fps{0};
        /// @brief 目标帧时间
        double _target_frame_time{0.0f};

    public:
        Time();
        Time(const Time &) = delete;
        Time(Time &&) = delete;
        Time &operator=(const Time &) = delete;
        Time &operator=(Time &&) = delete;
        /// @brief 每帧开始调用 更新内部时间并计算状态
        void update();

        float getDeltaTime() const;
        float getUnScaledDeltaTime() const;
        float getTimeScale() const { return _time_scale; }
        void setTimeScale(float scale);
        int getTargetFps() const { return _target_fps; }
        void setTargetFps(int fps);

    private:
        /// @brief 限制帧率
        /// @param current_delta_time 当前帧执行时间
        void limitFrameRate(float current_delta_time);
    };
}