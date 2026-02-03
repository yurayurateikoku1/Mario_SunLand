#include "ui_element.h"
#include "../core/context.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <utility>

engine::ui::UIElement::UIElement(const glm::vec2 &position, const glm::vec2 &size)
    : _position(position), _size(size)
{
}

void engine::ui::UIElement::update(float dt, engine::core::Context &context)
{
    if (!_visible)
    {
        return;
    }
    for (auto it = _children.begin(); it != _children.end();)
    {
        if (*it && !(*it)->isNeedRemove())
        {
            (*it)->update(dt, context);
            ++it;
        }
        else
        {
            it = _children.erase(it);
        }
    }
}

void engine::ui::UIElement::render(engine::core::Context &context)
{
    if (!_visible)
    {
        return;
    }
    for (const auto &child : _children)
    {
        if (child)
        {
            child->render(context);
        }
    }
}

bool engine::ui::UIElement::handleInput(engine::core::Context &context)
{
    if (!_visible)
    {
        return false;
    }

    for (auto it = _children.begin(); it != _children.end();)
    {
        if (*it && !(*it)->isNeedRemove())
        {
            if ((*it)->handleInput(context))
            {
                return true;
            }
            ++it;
        }
        else
        {
            it = _children.erase(it);
        }
    }
    return false;
}

void engine::ui::UIElement::addChild(std::unique_ptr<UIElement> child)
{
    if (child)
    {
        child->setParent(this);
        _children.push_back(std::move(child));
    }
}

std::unique_ptr<engine::ui::UIElement> engine::ui::UIElement::removeChild(UIElement *child_ptr)
{
    auto it = std::find_if(_children.begin(), _children.end(), [child_ptr](const std::unique_ptr<UIElement> &child)
                           { return child.get() == child_ptr; });
    if (it != _children.end())
    {
        std::unique_ptr<UIElement> remove_child = std::move(*it);
        _children.erase(it);
        remove_child->setParent(nullptr);
        return remove_child;
    }
    return nullptr;
}

void engine::ui::UIElement::removeAllChildren()
{
    for (auto &child : _children)
    {
        child->setParent(nullptr);
    }
    _children.clear();
}

engine::utils::Rect engine::ui::UIElement::getBounds() const
{
    auto abs_pos = getScreenPosition();
    return engine::utils::Rect(abs_pos, _size);
}

glm::vec2 engine::ui::UIElement::getScreenPosition() const
{
    if (_parent)
    {
        return _parent->getScreenPosition() + _position;
    }
    return _position;
}

bool engine::ui::UIElement::isPointInside(const glm::vec2 &point) const
{
    auto bounds = getBounds();
    return (point.x >= bounds.position.x && point.x < (bounds.position.x + bounds.size.x) &&
            point.y >= bounds.position.y &&
            point.y < (bounds.position.y + bounds.size.y));
}
