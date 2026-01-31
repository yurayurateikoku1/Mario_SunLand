#pragma once
#include "../../engine/scene/scene.h"
#include <memory>
namespace engine::object
{
    class GameObject;
}
namespace game::scene
{
    /// @brief 主要的游戏场景
    class GameScene final : public engine::scene::Scene
    {
        engine::object::GameObject *_player{nullptr};

    public:
        GameScene(const std::string &name, engine::core::Context &context, engine::scene::SceneManager &scene_manager);
        void init() override;
        void update(float dt) override;
        void render() override;
        void handleInput() override;
        void clean() override;

    private:
        [[nodiscard]] bool initlevel();
        [[nodiscard]] bool initplayer();
    };
}