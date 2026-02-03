#pragma once
#include "ui_element.h"
#include <optional>
#include "../utils/math.h"

namespace engine::ui
{

    class UIPanel final : public UIElement
    {
        std::optional<engine::utils::FColor> _background_color;

    public:
        UIPanel() = default;
        UIPanel(const glm::vec2 &position = {0.0f, 0.0f}, const glm::vec2 &size = {0.0f, 0.0f}, std::optional<engine::utils::FColor> background_color = std::nullopt);
        void setBackgroundColor(const std::optional<engine::utils::FColor> &background_color) { _background_color = background_color; }
        const std::optional<engine::utils::FColor> &getBackgroundColor() const { return _background_color; }
        void render(engine::core::Context &context) override;
        virtual ~UIPanel() = default;
    };

}
