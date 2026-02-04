#pragma once
#include "../../engine/scene/scene.h"

namespace game::scene
{
    class SplashScene final : public engine::scene::Scene
    {
    private:
        float _elapsed_time{0.0f};
        static constexpr float SPLASH_DURATION = 2.0f;

    public:
        SplashScene(engine::core::Context &context, engine::scene::SceneManager &scene_manager);
        ~SplashScene() override = default;

        void init() override;
        void update(float dt) override;

        SplashScene(const SplashScene &) = delete;
        SplashScene &operator=(const SplashScene &) = delete;
        SplashScene(SplashScene &&) = delete;
        SplashScene &operator=(SplashScene &&) = delete;
    };
}
