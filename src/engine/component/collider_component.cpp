#include "collider_component.h"
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>
#include "transform_component.h"
#include "../object/game_object.h"
#include "../physics/physics_engine.h"
engine::component::ColliderComponent::ColliderComponent(std::unique_ptr<engine::physics::Collider> collider, engine::utils::Alignment alignment, bool is_trigger, bool is_active)
    : _collider(std::move(collider)), _alignment(alignment), _is_trigger(is_trigger), _is_active(is_active)
{
    if (!_collider)
    {
        spdlog::error("Collider is not initialized.");
    }
}

void engine::component::ColliderComponent::updateOffset()
{
    if (!_collider)
    {
        return;
    }
    // 获取碰撞盒的大小
    auto collider_size = _collider->getAABBSize();

    // 如果尺寸无效,返回0
    if (collider_size.x <= 0.0f || collider_size.y <= 0.0f)
    {
        _offset = {0.0f, 0.0f};
        return;
    }
    auto scale = _transform->getScale();
    switch (_alignment)
    {
    case engine::utils::Alignment::TOP_LEFT:
        _offset = glm::vec2{0.0f, 0.0f} * scale;
        break;
    case engine::utils::Alignment::TOP_CENTER:
        _offset = glm::vec2{-collider_size.x / 2.0f, 0.0f} * scale;
        break;
    case engine::utils::Alignment::TOP_RIGHT:
        _offset = glm::vec2{-collider_size.x, 0.0f} * scale;
        break;
    case engine::utils::Alignment::CENTER_LEFT:
        _offset = glm::vec2{0.0f, -collider_size.y / 2.0f} * scale;
        break;
    case engine::utils::Alignment::CENTER:
        _offset = glm::vec2{-collider_size.x / 2.0f, -collider_size.y / 2.0f} * scale;
        break;
    case engine::utils::Alignment::CENTER_RIGHT:
        _offset = glm::vec2{-collider_size.x, -collider_size.y / 2.0f} * scale;
        break;
    case engine::utils::Alignment::BOTTOM_LEFT:
        _offset = glm::vec2{0.0f, -collider_size.y} * scale;
        break;
    case engine::utils::Alignment::BOTTOM_CENTER:
        _offset = glm::vec2{-collider_size.x / 2.0f, -collider_size.y} * scale;
        break;
    case engine::utils::Alignment::BOTTOM_RIGHT:
        _offset = glm::vec2{-collider_size.x, -collider_size.y} * scale;
        break;
    default:
        break;
    }
}

engine::utils::Rect engine::component::ColliderComponent::getWorldAABB() const
{
    if (!_transform || !_collider)
    {
        return {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
    }
    const glm::vec2 top_left_pos = _transform->getPosition() + _offset;
    const glm::vec2 base_size = _collider->getAABBSize();
    const glm::vec2 scale = _transform->getScale();
    glm::vec2 scaled_size = base_size * scale;
    return {top_left_pos, scaled_size};
}

void engine::component::ColliderComponent::setAlignment(engine::utils::Alignment alignment)
{
    _alignment = alignment;
    if (_transform && _collider)
    {
        updateOffset();
    }
}

void engine::component::ColliderComponent::init()
{
    if (!_owner)
    {
        spdlog::error("ColliderComponent: Owner is not set.");
        return;
    }

    _transform = _owner->getComponent<engine::component::TransformComponent>();
    if (!_transform)
    {
        spdlog::error("ColliderComponent: Transform component is not set.");
        return;
    }

    updateOffset();
}
