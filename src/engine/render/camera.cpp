#include "camera.h"
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
