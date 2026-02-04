#include "ui_interactive.h"
#include "state/ui_state.h"
#include "../core/context.h"
#include "../render/render.h"
#include "../resource/resource_manager.h"
#include "../audio/audio_player.h"
#include <spdlog/spdlog.h>

engine::ui::UIInteractive::UIInteractive(engine::core::Context &context, glm::vec2 position, glm::vec2 size)
    : UIElement(std::move(position), std::move(size)), _context(context)
{
}

engine::ui::UIInteractive::~UIInteractive() = default;

void engine::ui::UIInteractive::addSprite(const std::string &name, std::unique_ptr<engine::render::Sprite> sprite)
{
    // 可交互UI元素必须有一个size用于交互检测，因此如果参数列表中没有指定，则用图片大小作为size
    if (_size.x == 0.0f && _size.y == 0.0f)
    {
        _size = _context.getResourceManager().getTextureSize(sprite->getTextureId());
    }
    // 添加精灵
    _sprites[std::string(name)] = std::move(sprite);
}

void engine::ui::UIInteractive::setSprite(const std::string &name)
{
    if (_sprites.find(std::string(name)) != _sprites.end())
    {
        _current_sprite = _sprites[std::string(name)].get();
    }
    else
    {
        spdlog::warn("Sprite '{}' not found", name);
    }
}

void engine::ui::UIInteractive::addSound(const std::string &name, const std::string &path)
{
    _sounds[std::string(name)] = path;
}

void engine::ui::UIInteractive::playSound(const std::string &name)
{
    if (_sounds.find(std::string(name)) != _sounds.end())
    {
        _context.getAudioPlayer().playSound(_sounds[std::string(name)]);
    }
    else
    {
        spdlog::error("Sound '{}' 未找到", name);
    }
}

void engine::ui::UIInteractive::setState(std::unique_ptr<engine::ui::state::UIState> state)
{
    if (!state)
    {
        spdlog::warn("state is nullptr");
        return;
    }

    _state = std::move(state);
    _state->enter();
}

bool engine::ui::UIInteractive::handleInput(engine::core::Context &context)
{
    if (UIElement::handleInput(context))
    {
        return true;
    }

    // 先更新子节点，再更新自己（状态）
    if (_state && _interactive)
    {
        if (auto next_state = _state->handleInput(context); next_state)
        {
            setState(std::move(next_state));
            return true;
        }
    }
    return false;
}

void engine::ui::UIInteractive::render(engine::core::Context &context)
{
    if (!_visible)
        return;

    // 先渲染自身
    context.getRender().drawUISprite(*_current_sprite, getScreenPosition(), _size);

    UIElement::render(context);
}
