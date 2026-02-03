#pragma once
#include <SDL3/SDL_rect.h>
#include <memory>
#include <vector>
#include "../utils/math.h"

namespace engine::core
{
    class Context;
} // namespace

namespace engine::ui
{
    class UIElement
    {
    protected:
        glm::vec2 _position;
        glm::vec2 _size;
        bool _visible = true;
        bool _need_remove = false;
        UIElement *_parent = nullptr;
        std::vector<std::unique_ptr<UIElement>> _children;

    public:
        explicit UIElement(const glm::vec2 &position = {0.0f, 0.0f}, const glm::vec2 &size = {0.0f, 0.0f});
        virtual ~UIElement() = default;
        UIElement(const UIElement &) = delete;
        UIElement(UIElement &&) = delete;
        UIElement &operator=(const UIElement &) = delete;
        UIElement &operator=(UIElement &&) = delete;

        virtual void update(float dt, engine::core::Context &context);
        virtual void render(engine::core::Context &context);
        virtual bool handleInput(engine::core::Context &context);

        void addChild(std::unique_ptr<UIElement> child);
        std::unique_ptr<UIElement> removeChild(UIElement *child_ptr);
        void removeAllChildren();

        const glm::vec2 &getPosition() const { return _position; }
        const glm::vec2 &getSize() const { return _size; }
        bool isVisible() const { return _visible; }
        bool isNeedRemove() const { return _need_remove; }
        UIElement *getParent() const { return _parent; }
        const std::vector<std::unique_ptr<UIElement>> &getChildren() const { return _children; }

        void setSize(const glm::vec2 &size) { _size = size; }
        void setVisible(bool visible) { _visible = visible; }
        void setParent(UIElement *parent) { _parent = parent; }
        void setPosition(const glm::vec2 &position) { _position = position; }
        void setNeedRemove(bool need_remove) { _need_remove = need_remove; }

        engine::utils::Rect getBounds() const;
        glm::vec2 getScreenPosition() const;
        bool isPointInside(const glm::vec2 &point) const;
    };
}
