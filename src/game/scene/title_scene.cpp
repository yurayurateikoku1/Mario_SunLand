#include "title_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/utils/math.h"
#include "../data/session_data.h"
#include "game_scene.h"
#include "helps_scene.h"
#include <spdlog/spdlog.h>

namespace game::scene
{

    // 构造函数：初始化场景名称和上下文，创建 UI 管理器
    TitleScene::TitleScene(engine::core::Context &context,
                           engine::scene::SceneManager &scene_manager,
                           std::shared_ptr<game::data::SessionData> session_data)
        : engine::scene::Scene("TitleScene", context, scene_manager),
          _session_data(std::move(session_data))
    {
        if (!_session_data)
        {
            spdlog::warn("TitleScene session_data null.");
            _session_data = std::make_shared<game::data::SessionData>();
        }
        spdlog::trace("TitleScene created.");
    }

    // 初始化场景
    void TitleScene::init()
    {
        if (_is_initialized)
        {
            return;
        }
        // 加载背景地图
        engine::scene::LevelLoader level_loader;
        if (!level_loader.loadLevel("assets/maps/level0.tmj", *this))
        {
            spdlog::error("loadLevel failed.");
            return;
        }
        _session_data->syncHighScore("assets/save.json"); // 更新最高分

        // 重置相机坐标，不限制边界
        _context.getCamera().setPosition(glm::vec2(0.0f, 0.0f));
        _context.getCamera().setLimitBounds(std::nullopt); // 若无这一行，从GameScene返回到标题场景时，相机会限制在地图边界内

        // 创建 UI 元素
        createUI();

        Scene::init();
        spdlog::trace("TitleScene initialized.");
    }

    // 创建 UI 界面元素
    void TitleScene::createUI()
    {
        spdlog::trace("Creating TitleScene UI...");
        _context.getGameState().setState(engine::core::State::Title);
        auto window_size = _context.getGameState().getLogicalSize();

        if (!_ui_manager->init(window_size))
        {
            spdlog::error("Failed to initialize UI manager.");
            return;
        }

        // 设置背景音乐
        _context.getAudioPlayer().playMusic("assets/audio/platformer_level03_loop.ogg");

        // 创建标题图片 (假设不知道大小)
        auto title_image = std::make_unique<engine::ui::UIImage>("assets/textures/UI/title-screen.png");
        auto size = _context.getResourceManager().getTextureSize(title_image->getTextureId());
        title_image->setSize(size * 2.0f); // 放大为2倍

        // 水平居中
        auto title_pos = (window_size - title_image->getSize()) / 2.0f - glm::vec2(0.0f, 50.0f);
        title_image->setPosition(title_pos);
        _ui_manager->addElement(std::move(title_image));

        // --- 创建按钮面板并居中 --- (4个按钮，设定好大小、间距)
        float button_width = 96.0f;
        float button_height = 32.0f;
        float button_spacing = 20.0f;
        int num_buttons = 4;

        // 计算面板总宽度
        float panel_width = num_buttons * button_width + (num_buttons - 1) * button_spacing;
        float panel_height = button_height;

        // 计算面板位置使其居中
        float panel_x = (window_size.x - panel_width) / 2.0f;
        float panel_y = window_size.y * 0.65f; // 垂直位置中间靠下

        auto button_panel = std::make_unique<engine::ui::UIPanel>(
            glm::vec2(panel_x, panel_y),
            glm::vec2(panel_width, panel_height));

        // --- 创建按钮并添加到 UIPanel (位置是相对于 UIPanel 的 0,0) ---
        glm::vec2 current_button_pos = glm::vec2(0.0f, 0.0f);
        glm::vec2 button_size = glm::vec2(button_width, button_height);

        // Start Button
        auto start_button = std::make_unique<engine::ui::UIButton>(_context,
                                                                   "assets/textures/UI/buttons/Start1.png",
                                                                   "assets/textures/UI/buttons/Start2.png",
                                                                   "assets/textures/UI/buttons/Start3.png",
                                                                   current_button_pos,
                                                                   button_size,
                                                                   [this]()
                                                                   { this->onStartGameClick(); });
        button_panel->addChild(std::move(start_button));

        // Load Button
        current_button_pos.x += button_width + button_spacing;
        auto load_button = std::make_unique<engine::ui::UIButton>(_context,
                                                                  "assets/textures/UI/buttons/Load1.png",
                                                                  "assets/textures/UI/buttons/Load2.png",
                                                                  "assets/textures/UI/buttons/Load3.png",
                                                                  current_button_pos,
                                                                  button_size,
                                                                  [this]()
                                                                  { this->onLoadGameClick(); });
        button_panel->addChild(std::move(load_button));

        // Helps Button
        current_button_pos.x += button_width + button_spacing;
        auto helps_button = std::make_unique<engine::ui::UIButton>(_context,
                                                                   "assets/textures/UI/buttons/Helps1.png",
                                                                   "assets/textures/UI/buttons/Helps2.png",
                                                                   "assets/textures/UI/buttons/Helps3.png",
                                                                   current_button_pos,
                                                                   button_size,
                                                                   [this]()
                                                                   { this->onHelpsClick(); });
        button_panel->addChild(std::move(helps_button));

        // Quit Button
        current_button_pos.x += button_width + button_spacing;
        auto quit_button = std::make_unique<engine::ui::UIButton>(_context,
                                                                  "assets/textures/UI/buttons/Quit1.png",
                                                                  "assets/textures/UI/buttons/Quit2.png",
                                                                  "assets/textures/UI/buttons/Quit3.png",
                                                                  current_button_pos,
                                                                  button_size,
                                                                  [this]()
                                                                  { this->onQuitClick(); });
        button_panel->addChild(std::move(quit_button));

        // 将 UIPanel 添加到UI管理器
        _ui_manager->addElement(std::move(button_panel));

        // 创建 Credits 标签
        auto credits_label = std::make_unique<engine::ui::UILabel>(_context.getTextRenderer(),
                                                                   " Credits: Yurayurateukoku - 2025",
                                                                   "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                   16,
                                                                   engine::utils::FColor{0.8f, 0.8f, 0.8f, 1.0f});
        credits_label->setPosition(glm::vec2{(window_size.x - credits_label->getSize().x) / 2.0f,
                                             window_size.y - credits_label->getSize().y - 10.0f});
        _ui_manager->addElement(std::move(credits_label));

        spdlog::trace("TitleScene UI created successfully.");
    }

    // 更新场景逻辑
    void TitleScene::update(float delta_time)
    {
        Scene::update(delta_time);

        // 相机自动向右移动
        _context.getCamera().move(glm::vec2(delta_time * 100.0f, 0.0f));
    }

    // --- 按钮回调实现 --- //

    void TitleScene::onStartGameClick()
    {
        if (_session_data)
        {
            _session_data->reset();
        }
        _scene_manager.requestReplaceScene(std::make_unique<GameScene>(_context, _scene_manager, _session_data));
    }

    void TitleScene::onLoadGameClick()
    {
        if (!_session_data)
        {
            return;
        }

        if (_session_data->loadFromFile("assets/save.json"))
        {
            _scene_manager.requestReplaceScene(std::make_unique<GameScene>(_context, _scene_manager, _session_data));
        }
        else
        {
            spdlog::warn("Failed to load game data.");
        }
    }

    void TitleScene::onHelpsClick()
    {

        _scene_manager.requestPushScene(std::make_unique<HelpsScene>(_context, _scene_manager));
    }

    void TitleScene::onQuitClick()
    {
        _session_data->syncHighScore("assets/save.json"); // 退出前先同步最高分
        _context.getInputManager().setShouldQuit(true);
    }

} // namespace game::scenes