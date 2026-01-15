#include "transform_component.h"
#include "collider_component.h"
#include <spdlog/spdlog.h>
#include "../object/game_object.h"
#include "sprite_component.h"

void engine::component::TransformComponent::setScale(const glm::vec2 &scale)
{
    _scale = scale;
    if (_owner)
    {
        auto sprite_comp = _owner->getComponent<engine::component::SpriteComponent>();
        if (sprite_comp)
        {
            /* code */
            sprite_comp->updateOffset(); // 更新偏移
        }
        auto collider_comp = _owner->getComponent<engine::component::ColliderComponent>();
        if (collider_comp)
        {
            collider_comp->updateOffset();
        }
    }
}
