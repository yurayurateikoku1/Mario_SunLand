#include "resource_manager.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include "texture_manager.h"
#include "audio_manager.h"
#include "font_manager.h"
engine::resource::ResourceManager::ResourceManager(SDL_Renderer *renderer)
{
    _texture_manager = std::make_unique<TextureManager>(renderer);
    _audio_manager = std::make_unique<AudioManager>();
    _font_manager = std::make_unique<FontManager>();

    spdlog::info("ResourceManager init successfully");
}

engine::resource::ResourceManager::~ResourceManager() = default;

void engine::resource::ResourceManager::clear()
{
    _texture_manager->clearTextures();
    _audio_manager->clearSounds();
    _audio_manager->clearMusics();
    _font_manager->clearFonts();
}

SDL_Texture *engine::resource::ResourceManager::loadTexture(const std::string &file_path)
{
    return _texture_manager->loadTexture(file_path);
}

SDL_Texture *engine::resource::ResourceManager::getTexture(const std::string &file_path)
{
    return _texture_manager->getTexture(file_path);
}

void engine::resource::ResourceManager::unloadTexture(const std::string &file_path)
{
    _texture_manager->unloadTexture(file_path);
}

glm::vec2 engine::resource::ResourceManager::getTextureSize(const std::string &file_path)
{
    return _texture_manager->getTextureSize(file_path);
}
void engine::resource::ResourceManager::clearTextures()
{
    _texture_manager->clearTextures();
}

MIX_Audio *engine::resource::ResourceManager::loadSound(const std::string &file_path)
{
    return _audio_manager->loadSound(file_path);
}

MIX_Audio *engine::resource::ResourceManager::getSound(const std::string &file_path)
{
    return _audio_manager->getSound(file_path);
}

void engine::resource::ResourceManager::unloadSound(const std::string &file_path)
{
    _audio_manager->unloadSound(file_path);
}

void engine::resource::ResourceManager::clearSounds()
{
    _audio_manager->clearSounds();
}

MIX_Audio *engine::resource::ResourceManager::loadMusic(const std::string &file_path)
{
    return _audio_manager->loadMusic(file_path);
}

MIX_Audio *engine::resource::ResourceManager::getMusic(const std::string &file_path)
{
    return _audio_manager->getMusic(file_path);
}

void engine::resource::ResourceManager::unloadMusic(const std::string &file_path)
{
    _audio_manager->unloadMusic(file_path);
}

void engine::resource::ResourceManager::clearMusics()
{
    _audio_manager->clearMusics();
}

TTF_Font *engine::resource::ResourceManager::loadFont(const std::string &file_path, int font_size)
{
    return _font_manager->loadFont(file_path, font_size);
}

TTF_Font *engine::resource::ResourceManager::getFont(const std::string &file_path, int font_size)
{
    return _font_manager->getFont(file_path, font_size);
}

void engine::resource::ResourceManager::unloadFont(const std::string &file_path, int font_size)
{
    _font_manager->unloadFont(file_path, font_size);
}

void engine::resource::ResourceManager::clearFonts()
{
    _font_manager->clearFonts();
}

void engine::resource::ResourceManager::playSound(const std::string &file_path, int loops)
{
    _audio_manager->playSound(file_path, loops);
}

void engine::resource::ResourceManager::playMusic(const std::string &file_path, int loops)
{
    _audio_manager->playMusic(file_path, loops);
}

void engine::resource::ResourceManager::stopMusic()
{
    _audio_manager->stopMusic();
}

void engine::resource::ResourceManager::stopSound()
{
    _audio_manager->stopSound();
}

void engine::resource::ResourceManager::pauseMusic()
{
    _audio_manager->pauseMusic();
}

void engine::resource::ResourceManager::pauseSound()
{
    _audio_manager->pauseSound();
}

void engine::resource::ResourceManager::resumeMusic()
{
    _audio_manager->resumeMusic();
}

void engine::resource::ResourceManager::resumeSound()
{
    _audio_manager->resumeSound();
}

void engine::resource::ResourceManager::setMusicGain(float gain)
{
    _audio_manager->setMusicGain(gain);
}

void engine::resource::ResourceManager::setSoundGain(float gain)
{
    _audio_manager->setSoundGain(gain);
}