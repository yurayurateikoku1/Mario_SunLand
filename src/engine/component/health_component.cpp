#include "health_component.h"
#include "../../engine/object/game_object.h"
#include <spdlog/spdlog.h>
#include <glm/common.hpp>
engine::component::HealthComponent::HealthComponent(int max_health, float invincible_duration)
    : _max_health(glm::max(max_health, 1)), _invincible_duration(invincible_duration), _current_health(max_health)
{
}

bool engine::component::HealthComponent::takeDamage(int damage_amount)
{
    if (damage_amount <= 0 || !isAlive())
    {
        return false;
    }
    if (_is_invincible)
    {
        return false;
    }
    _current_health -= damage_amount;
    _current_health = std::max(_current_health, 0);
    if (isAlive() && _invincible_duration > 0.0f)
    {
        setInvincible(_invincible_duration);
    }
    return true;
}

void engine::component::HealthComponent::heal(int heal_amount)
{
    if (heal_amount <= 0 || !isAlive())
    {
        return;
    }
    _current_health += heal_amount;
    _current_health = std::min(_current_health, _max_health);
}

void engine::component::HealthComponent::setCurrentHealth(int new_health)
{
    _current_health = glm::max(0, glm::min(_current_health, _max_health));
}

void engine::component::HealthComponent::setMaxHealth(int new_max_health)
{
    _max_health = glm::max(new_max_health, 1);
    _current_health = std::min(_current_health, _max_health);
}

void engine::component::HealthComponent::setInvincible(float duration)
{
    if (duration > 0)
    {
        _is_invincible = true;
        _invincible_timer = duration;
    }
    else
    {
        _is_invincible = false;
        _invincible_timer = 0.0f;
    }
}

void engine::component::HealthComponent::update(float delta_time, engine::core::Context &)
{
    if (_is_invincible)
    {
        _invincible_timer -= delta_time;
        if (_invincible_timer <= 0.0f)
        {
            _is_invincible = false;
            _invincible_timer = 0.0f;
        }
    }
}
