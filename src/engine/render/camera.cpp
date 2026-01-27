#include "camera.h"
#include "../component/transform_component.h"
#include <spdlog/spdlog.h>
engine::render::Camera::Camera(const glm::vec2 &viewport_size, const glm::vec2 &position, const std::optional<engine::utils::Rect> &limit_bounds)
    : _viewport_size(viewport_size), _position(position), _limit_bounds(limit_bounds)
{
    spdlog::info("Camera created{},{}", _position.x, _position.y);
}

void engine::render::Camera::setPosition(const glm::vec2 &position)
{
    _position = position;
    clampPosition();
}

void engine::render::Camera::setLimitBounds(const std::optional<engine::utils::Rect> &limit_bounds)
{
    _limit_bounds = limit_bounds;
    clampPosition();
}

void engine::render::Camera::setTarget(engine::component::TransformComponent *target)
{
    _target = target;
}

engine::component::TransformComponent *engine::render::Camera::getTarget() const
{
    return _target;
}

void engine::render::Camera::clampPosition()
{
    if (_limit_bounds.has_value() && _limit_bounds->size.x > 0 && _limit_bounds->size.y > 0)
    {
        /* code */
        glm::vec2 min_camera_position = _limit_bounds->position;
        glm::vec2 max_camera_position = _limit_bounds->position + _limit_bounds->size - _viewport_size;

        max_camera_position.x = std::max(min_camera_position.x, max_camera_position.x);
        max_camera_position.y = std::max(min_camera_position.y, max_camera_position.y);
        _position = glm::clamp(_position, min_camera_position, max_camera_position);
    }
}

void engine::render::Camera::update(float delta)
{
    if (_target == nullptr)
        return;
    glm::vec2 target_pos = _target->getPosition();
    glm::vec2 desired_position = target_pos - _viewport_size / 2.0f; // 计算目标位置 (让目标位于视口中心)

    // 计算当前位置与目标位置的距离
    auto distance_ = glm::distance(_position, desired_position);
    constexpr float SNAP_THRESHOLD = 1.0f; // 设置一个距离阈值  (constexpr: 编译时常量，避免每次调用都计算)

    if (distance_ < SNAP_THRESHOLD)
    {
        // 如果距离小于阈值，直接吸附到目标位置
        _position = desired_position;
    }
    else
    {
        // 否则，使用线性插值平滑移动   glm::mix(a,b,t): 在向量 a 和 b 之间进行插值，t 是插值因子，范围在0到1之间。
        // 公式: (b-a)*t + a;   t = 0 时结果为 a，t = 1 时结果为 b
        _position = glm::mix(_position, desired_position, _smooth_speed * delta);
        _position = glm::vec2(glm::round(_position.x), glm::round(_position.y)); // 四舍五入到整数,省略的话偶尔会出现画面割裂
    }

    clampPosition();
}

void engine::render::Camera::move(const glm::vec2 &offset)
{
    _position += offset;
    clampPosition();
}

glm::vec2 engine::render::Camera::world2Screen(const glm::vec2 &world_position) const
{
    // 世界坐标减去相机位置，得到屏幕坐标
    return world_position - _position;
}

glm::vec2 engine::render::Camera::world2ScreenWithParallax(const glm::vec2 &world_position, const glm::vec2 &scroll_factor) const
{
    //
    return world_position - _position * scroll_factor;
}

glm::vec2 engine::render::Camera::screen2World(const glm::vec2 &screen_position) const
{
    // 屏幕坐标加上相机位置，得到世界坐标
    return screen_position + _position;
}
