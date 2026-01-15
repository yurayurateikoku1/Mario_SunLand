#pragma once
#include <vector>
#include <glm/vec2.hpp>

namespace engine::component
{
    class PhysicsComponent;
}

namespace engine::physics
{
    class PhysicsEngine
    {
    private:
        std::vector<engine::component::PhysicsComponent *> _physics_components;
        glm::vec2 _gravity = {0.0f, 980.0f};
        float _max_speed = 500.0f;

    public:
        PhysicsEngine() = default;

        PhysicsEngine(const PhysicsEngine &) = delete;
        PhysicsEngine(PhysicsEngine &&) = delete;
        PhysicsEngine &operator=(const PhysicsEngine &) = delete;
        PhysicsEngine &operator=(PhysicsEngine &&) = delete;

        void registerComponent(engine::component::PhysicsComponent *component);
        void unregisterComponent(engine::component::PhysicsComponent *component);

        void update(float dt);

        void setGravity(const glm::vec2 &gravity) { _gravity = gravity; }
        void setMaxSpeed(float max_speed) { _max_speed = max_speed; }
        const glm::vec2 &getGravity() const { return _gravity; }
        float getMaxSpeed() const { return _max_speed; }
    };

}
