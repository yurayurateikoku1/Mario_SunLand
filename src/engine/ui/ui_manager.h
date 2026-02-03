#pragma once
#include <memory>
#include <vector>
#include <glm/vec2.hpp>

namespace engine::core
{
    class Context;
}

namespace engine::ui
{
    class UIElement;
    class UIPanel;
}

namespace engine::ui
{
    class UIManager final
    {
    private:
        std::unique_ptr<engine::ui::UIPanel> _root_element;

    public:
        UIManager();
        ~UIManager();
        UIManager(const UIManager &) = delete;
        UIManager(UIManager &&) = delete;
        UIManager &operator=(const UIManager &) = delete;
        UIManager &operator=(UIManager &&) = delete;

        [[nodiscard]] bool init(const glm::vec2 &window_size);
        void addElement(std::unique_ptr<engine::ui::UIElement> element);
        UIPanel *getRootElement() const;
        void clearElements();

        bool handleInput(engine::core::Context &context);
        void update(float dt, engine::core::Context &context);
        void render(engine::core::Context &context);
    };
}