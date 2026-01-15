#include "physics_engine.h"
#include "collision.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include "../component/collider_component.h"
#include "../object/game_object.h"
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
    // 清空碰撞对
    _collision_pairs.clear();
    // 更新所有物理组件
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

        // 检查碰撞
        checkObjectCollision();
    }
}

void engine::physics::PhysicsEngine::checkObjectCollision()
{
    for (size_t i = 0; i < _physics_components.size(); i++)
    {
        auto *pc_a = _physics_components[i];
        if (!pc_a || !pc_a->getEnabled())
        {
            continue;
        }

        auto *obj_a = pc_a->getOwner();
        if (!obj_a)
        {
            continue;
        }
        auto *cc_a = obj_a->getComponent<engine::component::ColliderComponent>();
        if (!cc_a || !cc_a->isActive())
        {
            continue;
        }

        for (size_t j = i + 1; j < _physics_components.size(); j++)
        {
            auto *pc_b = _physics_components[j];
            if (!pc_b || !pc_b->getEnabled())
            {
                continue;
            }
            auto *obj_a = pc_a->getOwner();
            if (!obj_a)
            {
                continue;
            }
            auto *cc_a = obj_a->getComponent<engine::component::ColliderComponent>();
            if (!cc_a)
            {
                continue;
            }
            auto *obj_b = pc_b->getOwner();
            if (!obj_b)
            {
                continue;
            }
            auto *cc_b = obj_b->getComponent<engine::component::ColliderComponent>();
            if (!cc_b || !cc_b->isActive())
            {
                continue;
            }
            if (collision::checkCollision(*cc_a, *cc_b))
            {
                _collision_pairs.emplace_back(obj_a, obj_b);
            }
        }
    }
}
