#include "ui_manager.h"
#include "ui_panel.h"
#include "ui_element.h"
#include <spdlog/spdlog.h>
engine::ui::UIManager::UIManager()
{
    _root_element = std::make_unique<UIPanel>(glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
    spdlog::info("UIManager created");
}

engine::ui::UIManager::~UIManager() = default;

bool engine::ui::UIManager::init(const glm::vec2 &window_size)
{
    _root_element->setSize(window_size);
    spdlog::info("UIManager initialized");
    return true;
}

void engine::ui::UIManager::addElement(std::unique_ptr<engine::ui::UIElement> element)
{
    if (_root_element)
    {
        _root_element->addChild(std::move(element));
    }
    else
    {
        spdlog::error("UIManager is not initialized");
    }
}

engine::ui::UIPanel *engine::ui::UIManager::getRootElement() const
{
    return _root_element.get();
}

void engine::ui::UIManager::clearElements()
{
    if (_root_element)
    {
        _root_element->removeAllChildren();
    }
}

bool engine::ui::UIManager::handleInput(engine::core::Context &context)
{
    if (_root_element && _root_element->isVisible())
    {
        if (_root_element->handleInput(context))
        {
            return true;
        }
    }
    return false;
}

void engine::ui::UIManager::update(float dt, engine::core::Context &context)
{
    if (_root_element && _root_element->isVisible())
    {
        _root_element->update(dt, context);
    }
}

void engine::ui::UIManager::render(engine::core::Context &context)
{
    if (_root_element && _root_element->isVisible())
    {
        /* code */
        _root_element->render(context);
    }
}
