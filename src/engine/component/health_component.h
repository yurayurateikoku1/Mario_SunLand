#pragma once
#include "../../engine/component/component.h"

namespace engine::component
{

    class HealthComponent : public Component
    {
        friend class engine::object::GameObject;

    private:
        int _max_health = 1;
        int _current_health = 1;
        bool _is_invincible = false;
        float _invincible_duration = 0.0f;
        float _invincible_timer = 0.0f;

    public:
        explicit HealthComponent(int max_health = 1, float invincible_duration = 2.0f);
        ~HealthComponent() override = default;
        HealthComponent(const HealthComponent &) = delete;
        HealthComponent(HealthComponent &&) = delete;
        HealthComponent &operator=(const HealthComponent &) = delete;
        HealthComponent &operator=(HealthComponent &&) = delete;

        bool takeDamage(int damage_amount);
        void heal(int heal_amount);

        bool isAlive() const { return _current_health > 0; }
        bool isInvincible() const { return _is_invincible; }

        int getMaxHealth() const { return _max_health; }
        int getCurrentHealth() const { return _current_health; }

        void setCurrentHealth(int new_health);
        void setMaxHealth(int new_max_health);
        void setInvincible(float duration);
        void setInvincibleDuration(float duration) { _invincible_duration = duration; }

    protected:
        void update(float delta_time, engine::core::Context &) override;
    };
} // namespace engine::component
