#include "splash_scene.h"
#include "title_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_panel.h"
#include <spdlog/spdlog.h>

game::scene::SplashScene::SplashScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager)
    : Scene("SplashScene", context, scene_manager)
{
}

void game::scene::SplashScene::init()
{
    if (_is_initialized)
        return;

    glm::vec2 window_size{640.0f, 360.0f};
    _ui_manager->init(window_size);

    // Set black background
    _ui_manager->getRootElement()->setBackgroundColor(engine::utils::FColor{0.0f, 0.0f, 0.0f, 1.0f});

    // Create logo image and center it
    std::string logo_path = "assets/textures/pixel_empire_a.png";
    auto logo = std::make_unique<engine::ui::UIImage>(logo_path);
    auto logo_size = _context.getResourceManager().getTextureSize(logo_path) * 0.3f;
    logo->setSize(logo_size);
    logo->setPosition((window_size - logo_size) / 2.0f);
    _ui_manager->addElement(std::move(logo));

    Scene::init();
    spdlog::info("SplashScene initialized");
}

void game::scene::SplashScene::update(float dt)
{
    Scene::update(dt);
    _elapsed_time += dt;
    if (_elapsed_time >= SPLASH_DURATION)
    {
        _scene_manager.requestReplaceScene(
            std::make_unique<game::scene::TitleScene>(_context, _scene_manager));
    }
}
