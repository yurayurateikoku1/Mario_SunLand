#include "end_scene.h"
#include "title_scene.h"
#include "game_scene.h"
#include "../data/session_data.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/utils/math.h"
#include <spdlog/spdlog.h>

namespace game::scene
{

    EndScene::EndScene(engine::core::Context &context,
                       engine::scene::SceneManager &scene_manager,
                       std::shared_ptr<game::data::SessionData> session_data)
        : engine::scene::Scene("EndScene", context, scene_manager),
          _session_data(std::move(session_data))
    {
        if (!_session_data)
        {
            spdlog::error("EndScene session_data null.");
        }
    }

    void EndScene::init()
    {
        if (_is_initialized)
        {
            return;
        }

        // 设置游戏状态为 GameOver
        _context.getGameState().setState(engine::core::State::GameOver);

        createUI();

        Scene::init();
    }

    void EndScene::createUI()
    {
        auto window_size = _context.getGameState().getLogicalSize();
        if (!_ui_manager->init(window_size))
        {
            return;
        }
        auto is_win = _session_data->getIsWin();

        // --- 主文字标签 ---
        std::string main_message = is_win ? "YOU WIN! CONGRATS!" : "YOU DIED! TRY AGAIN!";
        // 赢了是绿色，输了是红色
        engine::utils::FColor message_color = is_win ? engine::utils::FColor{0.0f, 1.0f, 0.0f, 1.0f} : engine::utils::FColor{1.0f, 0.0f, 0.0f, 1.0f};

        auto main_label = std::make_unique<engine::ui::UILabel>(_context.getTextRenderer(),
                                                                main_message,
                                                                "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                48,
                                                                message_color);
        // 标签居中
        glm::vec2 label_size = main_label->getSize();
        glm::vec2 main_label_pos = {(window_size.x - label_size.x) / 2.0f, window_size.y * 0.3f};
        main_label->setPosition(main_label_pos);
        _ui_manager->addElement(std::move(main_label));

        // --- 得分标签 ---
        int current_score = _session_data->getCurrentScore();
        int high_score = _session_data->getHighScore();
        engine::utils::FColor score_color = {1.0f, 1.0f, 1.0f, 1.0f};
        int score_font_size = 24;

        // 当前得分
        std::string score_text = "Score: " + std::to_string(current_score);
        auto score_label = std::make_unique<engine::ui::UILabel>(_context.getTextRenderer(),
                                                                 score_text,
                                                                 "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                 score_font_size,
                                                                 score_color);
        glm::vec2 score_label_size = score_label->getSize();
        // x方向居中，y方向在主标签下方20像素
        glm::vec2 score_label_pos = {(window_size.x - score_label_size.x) / 2.0f, main_label_pos.y + label_size.y + 20.0f};
        score_label->setPosition(score_label_pos);
        _ui_manager->addElement(std::move(score_label));

        // --- 最高分 ---
        std::string high_score_text = "High Score: " + std::to_string(high_score);
        auto high_score_label = std::make_unique<engine::ui::UILabel>(_context.getTextRenderer(),
                                                                      high_score_text,
                                                                      "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                      score_font_size,
                                                                      score_color);
        glm::vec2 high_score_label_size = high_score_label->getSize();
        // x方向居中，y方向在当前得分下方10像素
        glm::vec2 high_score_label_pos = {(window_size.x - high_score_label_size.x) / 2.0f, score_label_pos.y + score_label_size.y + 10.0f};
        high_score_label->setPosition(high_score_label_pos);
        _ui_manager->addElement(std::move(high_score_label));

        // --- UI按钮 ---
        glm::vec2 button_size = {120.0f, 40.0f}; // 让按钮更大一点
        float button_spacing = 20.0f;
        float total_button_width = button_size.x * 2 + button_spacing;

        // 按钮放在右下角，与边缘间隔30像素
        float buttons_x = window_size.x - total_button_width - 30.0f;
        float buttons_y = window_size.y - button_size.y - 30.0f;

        // Back Button
        auto back_button = std::make_unique<engine::ui::UIButton>(_context,
                                                                  "assets/textures/UI/buttons/Back1.png",
                                                                  "assets/textures/UI/buttons/Back2.png",
                                                                  "assets/textures/UI/buttons/Back3.png",
                                                                  glm::vec2{buttons_x, buttons_y},
                                                                  button_size,
                                                                  [this]()
                                                                  { this->onBackClick(); });
        _ui_manager->addElement(std::move(back_button));

        // Restart Button
        buttons_x += button_size.x + button_spacing;
        auto restart_button = std::make_unique<engine::ui::UIButton>(_context,
                                                                     "assets/textures/UI/buttons/Restart1.png",
                                                                     "assets/textures/UI/buttons/Restart2.png",
                                                                     "assets/textures/UI/buttons/Restart3.png",
                                                                     glm::vec2{buttons_x, buttons_y},
                                                                     button_size,
                                                                     [this]()
                                                                     { this->onRestartClick(); });
        _ui_manager->addElement(std::move(restart_button));
    }

    void EndScene::onBackClick()
    {
        _scene_manager.requestReplaceScene(std::make_unique<TitleScene>(_context, _scene_manager, _session_data));
    }

    void EndScene::onRestartClick()
    {
        // 重新开始游戏
        _session_data->reset();
        _scene_manager.requestReplaceScene(std::make_unique<GameScene>(_context, _scene_manager, _session_data));
    }

} // namespace game::scene