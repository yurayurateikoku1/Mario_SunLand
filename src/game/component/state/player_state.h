#pragma once
#include <memory>
#include <string>

namespace engine::core
{
    class Context;
}

namespace game::component
{
    class PlayerComponent;
}

namespace game::component::state
{
    class PlayerState
    {
        friend class game::component::PlayerComponent;

    protected:
        PlayerComponent *_player_component = nullptr;

    public:
        PlayerState(PlayerComponent *player_component) : _player_component(player_component) {}
        virtual ~PlayerState() = default;
        PlayerState(const PlayerState &) = delete;
        PlayerState(PlayerState &&) = delete;
        PlayerState &operator=(const PlayerState &) = delete;
        PlayerState &operator=(PlayerState &&) = delete;

        void playerAnimation(const std::string &animation_name);

    protected:
        virtual void enter() = 0;
        virtual void exit() = 0;
        virtual std::unique_ptr<PlayerState> handleInput(engine::core::Context &) = 0;
        virtual std::unique_ptr<PlayerState> update(float dt, engine::core::Context &) = 0;
    };
}