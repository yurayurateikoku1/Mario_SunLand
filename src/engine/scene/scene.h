#pragma once
#include <vector>
#include <memory>
#include <string>

namespace engine::core
{
    class Context;
}

namespace engine::render
{
    class Renderer;
}

namespace engine::ui
{
    class UIManager;
}
namespace engine::input
{
    class InputManager;
}

namespace engine::object
{
    class GameObject;
}

namespace engine::scene
{
    class SceneManager;
    /// @brief 场景基类,复制场景中的游戏对象和场景的生命周期
    class Scene
    {
    protected:
        std::string _scene_name;
        engine::core::Context &_context;
        engine::scene::SceneManager &_scene_manager;
        std::unique_ptr<engine::ui::UIManager> _ui_manager;
        bool _is_initialized{false};
        /// @brief 场景中的游戏对象
        std::vector<std::unique_ptr<engine::object::GameObject>> _game_objects;
        /// @brief 将要添加的游戏对象
        std::vector<std::unique_ptr<engine::object::GameObject>> _pending_additions;

    public:
        /// @brief
        /// @param scene_name 场景名称
        /// @param context 上下文
        /// @param scene_manager 场景的管理器
        Scene(const std::string &scene_name, engine::core::Context &context, engine::scene::SceneManager &scene_manager);
        virtual ~Scene();
        Scene(const Scene &) = delete;
        Scene(Scene &&) = delete;
        Scene &operator=(const Scene &) = delete;
        Scene &operator=(Scene &&) = delete;

        virtual void init();
        virtual void update(float dt);
        virtual void render();
        virtual void handleInput();
        virtual void clean();

        /// @brief 添加游戏对象
        /// @param game_object
        virtual void addGameObject(std::unique_ptr<engine::object::GameObject> &&game_object);
        /// @brief 安全地添加游戏对象。（添加到pending_additions_中）
        /// @param game_object
        virtual void safeAddGameObject(std::unique_ptr<engine::object::GameObject> &&game_object);
        /// @brief  移除游戏对象
        /// @param game_object
        virtual void removeGameObject(engine::object::GameObject *game_object_ptr);
        /// @brief 安全地移除游戏对象
        virtual void safeRemoveGameObject(engine::object::GameObject *game_object_ptr);

        const std::vector<std::unique_ptr<engine::object::GameObject>> &getGameObjects() const { return _game_objects; }
        std::vector<std::unique_ptr<engine::object::GameObject>> &getGameObjects() { return _game_objects; }

        engine::object::GameObject *findGameObjectByName(const std::string &name) const;

        void setName(const std::string &name) { _scene_name = name; }
        std::string getName() const { return _scene_name; }
        void setInitialized(bool initialized) { _is_initialized = initialized; }
        bool getInitialized() const { return _is_initialized; }

        engine::core::Context &getContext() const { return _context; }
        engine::scene::SceneManager &getSceneManager() const { return _scene_manager; }

    protected:
        /// @brief 待处理的添加，每轮更新的最后调用
        void processPendingAdditions();
    };
}