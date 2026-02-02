#include "hurt_state.h"
#include "idle_state.h"
#include "walk_state.h"
#include "fall_state.h"
#include "../player_component.h"
#include "../../../engine/physics/physics_engine.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/object/game_object.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/audio_component.h"
void game::component::state::HurtState::enter()
{
    playerAnimation("hurt");

    auto physics_component = _player_component->getPhysicsComponent();
    auto sprite_component = _player_component->getSpriteComponent();

    auto knockback_velocity = glm::vec2(-100.0f, -150.0f);
    if (sprite_component->isFlipped())
    {
        knockback_velocity.x = -knockback_velocity.x;
    }
    physics_component->_velocity = knockback_velocity;
    if (auto *audio_component = _player_component->getAudioComponent(); audio_component)
    {
        audio_component->playerSound("hurt");
    }
}

void game::component::state::HurtState::exit()
{
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::HurtState::handleInput(engine::core::Context &)
{
    return nullptr;
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::HurtState::update(float dt, engine::core::Context &)
{
    _stunned_timer += dt;
    auto physics_component = _player_component->getPhysicsComponent();

    if (physics_component->getCollidedBelow())
    {
        /* code */
        if (glm::abs(physics_component->_velocity.x) < 1.0f)
        {
            return std::make_unique<IdleState>(_player_component);
        }
        else
        {
            return std::make_unique<WalkState>(_player_component);
        }
    }
    if (_stunned_timer > _player_component->getStunnedDuration())
    {
        _stunned_timer = 0.0f;
        return std::make_unique<FallState>(_player_component);
    }

    return nullptr;
}
