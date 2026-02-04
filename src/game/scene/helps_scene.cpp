#include "helps_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_image.h"
#include <spdlog/spdlog.h>

namespace game::scene
{

    HelpsScene::HelpsScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager)
        : engine::scene::Scene("HelpsScene", context, scene_manager)
    {
        spdlog::info("HelpsScene created.");
    }

    void HelpsScene::init()
    {
        if (_is_initialized)
        {
            return;
        }
        auto window_size = glm::vec2(640.0f, 360.0f);

        // 创建帮助图片 UIImage （让它覆盖整个屏幕）
        auto help_image = std::make_unique<engine::ui::UIImage>(
            "assets/textures/UI/instructions.png",
            glm::vec2(0.0f, 0.0f),
            window_size);

        _ui_manager->addElement(std::move(help_image));

        Scene::init();
        spdlog::info("HelpsScene initialized.");
    }

    void HelpsScene::handleInput()
    {
        if (!_is_initialized)
            return;

        // 检测是否按下鼠标左键
        if (_context.getInputManager().isActionPressed("MouseLeftClick"))
        {
            _scene_manager.requestPopScene();
        }
    }

} // namespace game::scene