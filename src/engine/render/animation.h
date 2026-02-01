#pragma once
#include <SDL3/SDL_rect.h>
#include <vector>
#include <string>

namespace engine::render
{
    /**
     * @brief 代表动画中的单个帧。
     */
    struct AnimationFrame
    {
        SDL_Rect source_rect; // 纹理图集上此帧的区域
        float duration;       // 持续时间
    };

    class Animation final
    {
    private:
        std::string _name;
        std::vector<AnimationFrame> _frames;
        float _total_duration;
        bool _loop = true;

    public:
        Animation(const std::string &name = "default", bool loop = true);
        ~Animation() = default;

        Animation(const Animation &) = delete;
        Animation(Animation &&) = delete;
        Animation &operator=(const Animation &) = delete;
        Animation &operator=(Animation &&) = delete;

        void addFrame(const SDL_Rect &source_rect, float duration);

        const AnimationFrame &getFrame(float time) const;

        const std::string &getName() const { return _name; }
        const std::vector<AnimationFrame> &getFrames() const { return _frames; };
        size_t getFrameCount() const { return _frames.size(); }
        float getTotalDuration() const { return _total_duration; }
        bool isLooping() const { return _loop; }
        bool isEmpty() const { return _frames.empty(); }

        void setName(const std::string &name) { _name = name; }
        void setLopping(bool loop) { _loop = loop; }
    };
}