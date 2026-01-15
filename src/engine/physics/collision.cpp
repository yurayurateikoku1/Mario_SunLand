#include "collision.h"
#include "../component/collider_component.h"
#include "../component/transform_component.h"
bool engine::physics::collision::checkCollision(engine::component::ColliderComponent &a, engine::component::ColliderComponent &b)
{
    auto a_collider = a.getCollider();
    auto b_collider = b.getCollider();
    auto a_transform = a.getTransform();
    auto b_transform = b.getTransform();

    auto a_size = a_collider->getAABBSize() * a_transform->getScale();
    auto b_size = b_collider->getAABBSize() * b_transform->getScale();
    auto a_pos = a_transform->getPosition() + a.getOffset();
    auto b_pos = b_transform->getPosition() + b.getOffset();
    if (!checkAABBOverlap(a_pos, a_size, b_pos, b_size))
    {
        return false;
    }
    if (a_collider->getType() == engine::physics::ColliderType::AABB && b_collider->getType() == engine::physics::ColliderType::AABB)
    {
        return true;
    }
    else if (a_collider->getType() == engine::physics::ColliderType::CIRCLE && b_collider->getType() == engine::physics::ColliderType::CIRCLE)
    {
        auto a_center = a_pos + a_size / 2.0f;
        auto b_center = b_pos + b_size / 2.0f;
        auto a_radius = a_size.x / 2.0f;
        auto b_radius = b_size.x / 2.0f;
        return checkCircleOverlap(a_center, a_radius, b_center, b_radius);
    }
    else if (a_collider->getType() == engine::physics::ColliderType::AABB && b_collider->getType() == engine::physics::ColliderType::CIRCLE)
    {
        auto b_center = b_pos + b_size / 2.0f;
        auto b_radius = b_size.x / 2.0f;
        auto nearest = glm::clamp(b_center, a_pos, a_pos + a_size);
        return checkPointInCircle(nearest, b_center, b_radius);
    }
    else if (a_collider->getType() == engine::physics::ColliderType::CIRCLE && b_collider->getType() == engine::physics::ColliderType::AABB)
    {
        auto a_center = a_pos + a_size / 2.0f;
        auto b_center = b_pos + b_size / 2.0f;
        auto a_radius = a_size.x / 2.0f;
        auto b_radius = b_size.x / 2.0f;
        return checkCircleOverlap(a_center, a_radius, b_center, b_radius);
    }
    return false;
}

bool engine::physics::collision::checkCircleOverlap(const glm::vec2 &center_a, float radius_a, const glm::vec2 &center_b, float radius_b)
{
    return (glm::length(center_a - center_b) < radius_a + radius_b);
}

bool engine::physics::collision::checkAABBOverlap(const glm::vec2 &pos_a, const glm::vec2 &size_a, const glm::vec2 &pos_b, const glm::vec2 &size_b)
{
    if (pos_a.x + size_a.x <= pos_b.x || pos_a.x >= pos_b.x + size_b.x || pos_a.y + size_a.y <= pos_b.y || pos_a.y >= pos_b.y + size_b.y)
    {
        return false;
    }
    return true;
}

bool engine::physics::collision::checkRectOverlap(const engine::utils::Rect &a, const engine::utils::Rect &b)
{
    return checkAABBOverlap(a.position, a.size, b.position, b.size);
}

bool engine::physics::collision::checkPointInCircle(const glm::vec2 &point, const glm::vec2 &center, float radius)
{
    return (glm::length(point - center) < radius);
}
