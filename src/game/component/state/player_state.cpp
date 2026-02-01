#include "player_state.h"
#include "../player_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/object/game_object.h"
#include <spdlog/spdlog.h>
void game::component::state::PlayerState::playerAnimation(const std::string &animation_name)
{
    if (!_player_component)
    {
        spdlog::error("Player component is null");
        return;
    }
    auto animation_component = _player_component->getAnimationComponent();
    if (!animation_component)
    {
        spdlog::error("Animation component is null");
        return;
    }
    animation_component->playAnimation(animation_name);
}