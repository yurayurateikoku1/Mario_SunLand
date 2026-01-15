#include "physics_engine.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>
void engine::physics::PhysicsEngine::registerComponent(engine::component::PhysicsComponent *component)
{
    _physics_components.push_back(component);
}

void engine::physics::PhysicsEngine::unregisterComponent(engine::component::PhysicsComponent *component)
{
    auto it = std::remove(_physics_components.begin(), _physics_components.end(), component);
    _physics_components.erase(it, _physics_components.end());
}

void engine::physics::PhysicsEngine::update(float dt)
{
    for (auto *pc : _physics_components)
    {
        if (!pc || !pc->getEnabled())
        {
            continue;
        }
        // 应用重力
        if (pc->getUseGravity())
        {
            pc->addForce(_gravity * pc->getMass());
        }
        // 更新速度
        pc->_velocity += (pc->getForce() / pc->getMass()) * dt;
        pc->clearForce();

        // 更新位置
        auto *tc = pc->getTransform();
        if (tc)
        {
            tc->translate(pc->_velocity * dt);
        }

        // 限制最大速度
        pc->_velocity = glm::clamp(pc->_velocity, -_max_speed, _max_speed);
    }
}
