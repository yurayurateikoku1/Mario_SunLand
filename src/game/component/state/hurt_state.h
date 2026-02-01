#pragma once
#include "player_state.h"

namespace game::component::state
{
    class HurtState : public PlayerState
    {
        friend class game::component::PlayerComponent;

    private:
        float _stunned_timer = 0.0f;

    public:
        HurtState(PlayerComponent *player_component) : PlayerState(player_component) {}
        ~HurtState() override = default;

    private:
        void enter() override;
        void exit() override;
        std::unique_ptr<PlayerState> handleInput(engine::core::Context &) override;
        std::unique_ptr<PlayerState> update(float dt, engine::core::Context &) override;
    };
}