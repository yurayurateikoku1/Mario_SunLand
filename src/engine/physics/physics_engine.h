#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <optional>
#include "../utils/math.h"
namespace engine::component
{
    class PhysicsComponent;
    class TileLayerComponent;
    enum class TileType;
}
namespace engine::object
{
    class GameObject;
}
namespace engine::physics
{
    class PhysicsEngine
    {
    private:
        std::vector<engine::component::PhysicsComponent *> _physics_components;
        glm::vec2 _gravity = {0.0f, 980.0f};
        float _max_speed = 500.0f;
        std::vector<std::pair<engine::object::GameObject *, engine::object::GameObject *>> _collision_pairs;
        std::vector<engine::component::TileLayerComponent *> _collision_tile_layers;
        std::optional<engine::utils::Rect> _world_bounds;

    public:
        PhysicsEngine() = default;

        PhysicsEngine(const PhysicsEngine &) = delete;
        PhysicsEngine(PhysicsEngine &&) = delete;
        PhysicsEngine &operator=(const PhysicsEngine &) = delete;
        PhysicsEngine &operator=(PhysicsEngine &&) = delete;

        void registerComponent(engine::component::PhysicsComponent *component);
        void unregisterComponent(engine::component::PhysicsComponent *component);

        void registerCollisionTileLayer(engine::component::TileLayerComponent *tile_layer);
        void unregisterCollisionTileLayer(engine::component::TileLayerComponent *tile_layer);

        void update(float dt);
        void checkObjectCollision();

        void resolveTileCollision(engine::component::PhysicsComponent *pc, float dt);

        void resolveSolidObjectCollisions(engine::object::GameObject *move_obj, engine::object::GameObject *solid_obj);

        void setGravity(const glm::vec2 &gravity) { _gravity = gravity; }
        void setMaxSpeed(float max_speed) { _max_speed = max_speed; }
        const glm::vec2 &getGravity() const { return _gravity; }
        float getMaxSpeed() const { return _max_speed; }

        void setWorldBounds(const engine::utils::Rect &world_bounds) { _world_bounds = world_bounds; }
        const std::optional<engine::utils::Rect> &getWorldBounds() const { return _world_bounds; }
        const std::vector<std::pair<engine::object::GameObject *, engine::object::GameObject *>> &getCollisionPairs() const { return _collision_pairs; }
        void applyWorldBounds(engine::component::PhysicsComponent *pc);

        float getTileHeightAtWidth(float width, engine::component::TileType tile_type, glm::vec2 tile_size);
    };

}
