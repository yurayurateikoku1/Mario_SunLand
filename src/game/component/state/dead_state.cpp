#include "dead_state.h"
#include "../player_component.h"
#include "../../../engine/physics/physics_engine.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/object/game_object.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/collider_component.h"

void game::component::state::DeadState::enter()
{
    playerAnimation("hurt");

    auto physics_component = _player_component->getPhysicsComponent();
    physics_component->_velocity = glm::vec2(0.0f, -200.0f);

    auto collider_component = _player_component->getOwner()->getComponent<engine::component::ColliderComponent>();
    if (collider_component)
    {
        collider_component->setActive(false);
    }
}

void game::component::state::DeadState::exit()
{
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::DeadState::handleInput(engine::core::Context &)
{
    return nullptr;
}

std::unique_ptr<game::component::state::PlayerState> game::component::state::DeadState::update(float dt, engine::core::Context &)
{
    return nullptr;
}
