#pragma once
#include <glm/vec2.hpp>

namespace engine::physics
{
    /// @brief 碰撞盒类型
    enum class ColliderType
    {
        NONE,
        AABB,
        CIRCLE
    };

    /// @brief 碰撞盒的抽象基类
    class Collider
    {
    protected:
        glm::vec2 _aabb_size = {0.0f, 0.0f};

    public:
        virtual ColliderType getType() const = 0;
        virtual ~Collider() = default;

        void setAABBSize(const glm::vec2 &size) { _aabb_size = size; }
        const glm::vec2 &getAABBSize() const { return _aabb_size; }
    };

    /// @brief 矩形碰撞盒
    class AABBCollider : public Collider
    {
    private:
        glm::vec2 _size = {0.0f, 0.0f};

    public:
        explicit AABBCollider(const glm::vec2 &size = {0.0f, 0.0f}) : _size(size) { setAABBSize(size); };
        ~AABBCollider() override = default;

        ColliderType getType() const override { return ColliderType::AABB; }
        const glm::vec2 &getSize() const { return _size; }
        void setSize(const glm::vec2 &size) { _size = size; }
    };

    class CircleCollider : public Collider
    {
    private:
        float _radius = 0.0f;

    public:
        explicit CircleCollider(float radius = 0.0f) : _radius(radius) { setAABBSize({radius, radius}); };
        ~CircleCollider() override = default;

        ColliderType getType() const override { return ColliderType::CIRCLE; }
        float getRadius() const { return _radius; }
        void setRadius(float radius) { _radius = radius; }
    };

}