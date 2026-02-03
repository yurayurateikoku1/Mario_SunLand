#pragma once
#include "../../engine/scene/scene.h"
#include <memory>
#include <glm/glm.hpp>
namespace engine::object
{
    class GameObject;
}
namespace game::data
{
    class SessionData;
}
namespace game::scene
{
    /// @brief 主要的游戏场景
    class GameScene final : public engine::scene::Scene
    {
        engine::object::GameObject *_player{nullptr};
        std::shared_ptr<game::data::SessionData> _game_session_data{nullptr};

    public:
        GameScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager, std::shared_ptr<game::data::SessionData> session_data = nullptr);
        void init() override;
        void update(float dt) override;
        void render() override;
        void handleInput() override;
        void clean() override;

    private:
        [[nodiscard]] bool initlevel();
        [[nodiscard]] bool initplayer();
        [[nodiscard]] bool initEnemyAndItem();
        [[nodiscard]] bool initUI();

        void handleObjectCollisions();
        void playerVsEnemyCollision(engine::object::GameObject *player, engine::object::GameObject *enemy);
        void playerVsItemCollision(engine::object::GameObject *player, engine::object::GameObject *item);

        void handleTileTriggers();
        void handlePlayerDamage(int damage);
        void createEffect(const glm::vec2 &center_pos, const std::string &tag);
        void toNextLevel(engine::object::GameObject *trigger);
        std::string levelNameToPath(const std::string &level_name) const { return "assets/maps/" + level_name + ".tmj"; };

        void testTextRenderer();
    };
}