#include "menu_scene.h"
#include "title_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/core/game_state.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/utils/math.h"
#include "../data/session_data.h"
#include <spdlog/spdlog.h>

namespace game::scene
{

    MenuScene::MenuScene(engine::core::Context &context,
                         engine::scene::SceneManager &scene_manager,
                         std::shared_ptr<game::data::SessionData> session_data)
        : Scene("MenuScene", context, scene_manager), // 调用基类构造函数
          _session_data(std::move(session_data))
    {
        if (!_session_data)
        {
            spdlog::error("MenuScene session_data null.");
        }
        spdlog::trace("MenuScene created.");
    }

    void MenuScene::init()
    {
        _context.getGameState().setState(engine::core::State::Paused);
        createUI();

        Scene::init();
        spdlog::trace("menuScene init.");
    }

    void MenuScene::createUI()
    {

        auto window_size = _context.getGameState().getLogicalSize();
        if (!_ui_manager->init(window_size))
        {
            spdlog::error("MenuScene UIManager init failed.");
            return;
        }

        // "PAUSE"标签
        auto pause_label = std::make_unique<engine::ui::UILabel>(_context.getTextRenderer(),
                                                                 "PAUSE",
                                                                 "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                 32);
        // 放在中间靠上的位置
        auto size = pause_label->getSize();
        auto label_y = window_size.y * 0.2;
        pause_label->setPosition(glm::vec2((window_size.x - size.x) / 2.0f, label_y));
        _ui_manager->addElement(std::move(pause_label));

        // --- 创建按钮 --- (4个按钮，设定好大小、间距)
        float button_width = 96.0f; // 按钮稍微小一点
        float button_height = 32.0f;
        float button_spacing = 10.0f;
        float start_y = label_y + 80.0f;                        // 从标签下方开始，增加间距
        float button_x = (window_size.x - button_width) / 2.0f; // 水平居中

        // Resume Button
        auto resume_button = std::make_unique<engine::ui::UIButton>(_context,
                                                                    "assets/textures/UI/buttons/Resume1.png",
                                                                    "assets/textures/UI/buttons/Resume2.png",
                                                                    "assets/textures/UI/buttons/Resume3.png",
                                                                    glm::vec2{button_x, start_y},
                                                                    glm::vec2{button_width, button_height},
                                                                    [this]()
                                                                    { this->onResumeClicked(); });
        _ui_manager->addElement(std::move(resume_button));

        // Save Button
        start_y += button_height + button_spacing;
        auto save_button = std::make_unique<engine::ui::UIButton>(_context,
                                                                  "assets/textures/UI/buttons/Save1.png",
                                                                  "assets/textures/UI/buttons/Save2.png",
                                                                  "assets/textures/UI/buttons/Save3.png",
                                                                  glm::vec2{button_x, start_y},
                                                                  glm::vec2{button_width, button_height},
                                                                  [this]()
                                                                  { this->onSaveClicked(); });
        _ui_manager->addElement(std::move(save_button));

        // Back Button
        start_y += button_height + button_spacing;
        auto back_button = std::make_unique<engine::ui::UIButton>(_context,
                                                                  "assets/textures/UI/buttons/Back1.png",
                                                                  "assets/textures/UI/buttons/Back2.png",
                                                                  "assets/textures/UI/buttons/Back3.png",
                                                                  glm::vec2{button_x, start_y},
                                                                  glm::vec2{button_width, button_height},
                                                                  [this]()
                                                                  { this->onBackClicked(); });
        _ui_manager->addElement(std::move(back_button));

        // Quit Button
        start_y += button_height + button_spacing;
        auto quit_button = std::make_unique<engine::ui::UIButton>(_context,
                                                                  "assets/textures/UI/buttons/Quit1.png",
                                                                  "assets/textures/UI/buttons/Quit2.png",
                                                                  "assets/textures/UI/buttons/Quit3.png",
                                                                  glm::vec2{button_x, start_y},
                                                                  glm::vec2{button_width, button_height},
                                                                  [this]()
                                                                  { this->onQuitClicked(); });
        _ui_manager->addElement(std::move(quit_button));
    }

    void MenuScene::handleInput()
    {
        // 先让 UIManager 处理交互
        Scene::handleInput();

        // 检查暂停键，允许按暂停键恢复游戏
        if (_context.getInputManager().isActionPressed("pause"))
        {
            _scene_manager.requestPopScene(); // 弹出自身以恢复底层的GameScene
            _context.getGameState().setState(engine::core::State::Playing);
        }
    }

    // --- 按钮回调函数实现 ---

    void MenuScene::onResumeClicked()
    {
        _scene_manager.requestPopScene(); // 弹出当前场景
        _context.getGameState().setState(engine::core::State::Playing);
    }

    void MenuScene::onSaveClicked()
    {

        if (_session_data && _session_data->saveToFile("assets/save.json"))
        {
            spdlog::info("game saved.");
        }
        else
        {
        }
    }

    void MenuScene::onBackClicked()
    {
        // 直接替换为TitleScene
        _scene_manager.requestReplaceScene(std::make_unique<TitleScene>(_context, _scene_manager, _session_data));
    }

    void MenuScene::onQuitClicked()
    {
        _session_data->syncHighScore("assets/save.json"); // 退出前先同步最高分
        _context.getInputManager().setShouldQuit(true);   // 输入管理器设置退出标志
    }

} // namespace game::scene