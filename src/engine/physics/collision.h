#pragma once
#include "../utils/math.h"

namespace engine::component
{
    class ColliderComponent;
}

namespace engine::physics::collision
{
    /// @brief 检测碰撞
    /// @param a
    /// @param b
    /// @return
    bool checkCollision(engine::component::ColliderComponent &a, engine::component::ColliderComponent &b);

    /// @brief 检测圆形碰撞
    /// @param center_a
    /// @param radius_a
    /// @param center_b
    /// @param radius_b
    /// @return
    bool checkCircleOverlap(const glm::vec2 &center_a, float radius_a, const glm::vec2 &center_b, float radius_b);

    /// @brief 检测AABB碰撞
    /// @param a
    /// @param b
    /// @return
    bool checkAABBOverlap(const glm::vec2 &pos_a, const glm::vec2 &size_a, const glm::vec2 &pos_b, const glm::vec2 &size_b);

    /// @brief 检测矩形碰撞
    /// @param a
    /// @param b
    /// @return
    bool checkRectOverlap(const engine::utils::Rect &a, const engine::utils::Rect &b);

    /// @brief 检测点是否在圆形内
    /// @param point
    /// @param center
    /// @param radius
    /// @return
    bool checkPointInCircle(const glm::vec2 &point, const glm::vec2 &center, float radius);
}