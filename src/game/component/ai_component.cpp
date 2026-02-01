#include "ai_component.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/animation_component.h"
void game::component::AIComponent::setBehavior(std::unique_ptr<ai::AIBehavior> behavior)
{
    _current_behavior = std::move(behavior);
    if (_current_behavior)
    {
        _current_behavior->enter(*this); // 调用新行为的enter方法
    }
}

bool game::component::AIComponent::takeDamage(int damage)
{
    bool success = false;
    if (auto *health_component = getOwner()->getComponent<engine::component::HealthComponent>(); health_component)
    {
        /* code */
        success = health_component->takeDamage(damage);
    }
    return success;
}

bool game::component::AIComponent::isAlive() const
{
    if (auto *health_component = getOwner()->getComponent<engine::component::HealthComponent>(); health_component)
    {
        /* code */
        return health_component->isAlive();
    }
    return true;
}

void game::component::AIComponent::init()
{
    _transform_component = getOwner()->getComponent<engine::component::TransformComponent>();
    _physics_component = getOwner()->getComponent<engine::component::PhysicsComponent>();
    _sprite_component = getOwner()->getComponent<engine::component::SpriteComponent>();
    _animation_component = getOwner()->getComponent<engine::component::AnimationComponent>();
}

void game::component::AIComponent::update(float delta_time, engine::core::Context &)
{
    if (_current_behavior)
    {
        _current_behavior->update(delta_time, *this); // 委托给策略
    }
}
