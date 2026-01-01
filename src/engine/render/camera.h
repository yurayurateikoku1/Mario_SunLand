#pragma once
#include "../utils/math.h"
#include <optional>

namespace engine::render
{
    /// @brief
    class Camera final
    {
    private:
        /// @brief 相机的左上角位置
        glm::vec2 _position;
        /// @brief 视口大小
        glm::vec2 _viewport_size;
        /// @brief 相机的限制区域
        std::optional<engine::utils::Rect> _limit_bounds;

    public:
        Camera(const glm::vec2 &viewport_size, const glm::vec2 &position = {0.0f, 0.0f}, const std::optional<engine::utils::Rect> &limit_bounds = std::nullopt);

        Camera(const Camera &) = delete;
        Camera &operator=(const Camera &) = delete;
        Camera(Camera &&) = delete;
        Camera &operator=(Camera &&) = delete;

        /// @brief 更新相机位置
        /// @param delta
        void update(float delta);

        /// @brief 移动相机
        /// @param offset
        void move(const glm::vec2 &offset);

        glm::vec2 world2Screen(const glm::vec2 &world_position) const;

        /// @brief 将世界坐标转换为屏幕坐标，带有视差
        /// @param world_position
        /// @param scroll_factor
        /// @return
        glm::vec2 world2ScreenWithParallax(const glm::vec2 &world_position, const glm::vec2 &scroll_factor) const;

        glm::vec2 screen2World(const glm::vec2 &screen_position) const;

        void setPosition(const glm::vec2 &position);
        void setLimitBounds(const std::optional<engine::utils::Rect> &limit_bounds);

        const glm::vec2 &getPosition() const { return _position; }
        const glm::vec2 &getViewportSize() const { return _viewport_size; }
        const std::optional<engine::utils::Rect> &getLimitBounds() const { return _limit_bounds; }

    private:
        /// @brief 限制相机在边界范围内
        void clampPosition();
    };
}