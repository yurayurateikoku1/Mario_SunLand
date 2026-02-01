#pragma once
#include "../../engine/scene/scene.h"
#include <memory>
#include <glm/glm.hpp>
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
        [[nodiscard]] bool initEnemyAndItem();

        void handleObjectCollisions();
        void playerVsEnemyCollision(engine::object::GameObject *player, engine::object::GameObject *enemy);
        void playerVsItemCollision(engine::object::GameObject *player, engine::object::GameObject *item);

        void handleTileTriggers();
        void createEffect(const glm::vec2 &center_pos, const std::string &tag);
    };
}