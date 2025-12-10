#include "SoundManager.h"

#include <iostream>

using namespace std;
using namespace sf;

SoundManager::SoundManager()
{
    if (!menuMusic.openFromFile("Assets/SFX/BackGround.mp3"))
        cerr << "Warning: menu_music.ogg not found\n";
    if (!gameMusic.openFromFile("Assets/SFX/BackGround.mp3"))
        cerr << "Warning: game_music.ogg not found\n";

    ensureBuffer("button_click", "Assets/SFX/button_click.mp3");
    ensureBuffer("jump", "Assets/SFX/jump.mp3");
    ensureBuffer("run", "Assets/SFX/run.ogg.opus", true);
    ensureBuffer("landing", "Assets/SFX/landing.mp3");
    ensureBuffer("rain", "Assets/SFX/rain.mp3");

    applyVolumes();
}

void SoundManager::ensureBuffer(const string& key, const string& path, bool loop)
{
    SoundBuffer buf;
    if (!buf.loadFromFile(path)) {
        cerr << "Warning: SFX " << path << " not found (key: " << key << ")\n";
    }
    buffers[key] = move(buf);
    sounds[key] = Sound();
    sounds[key].setBuffer(buffers[key]);
    sounds[key].setLoop(loop);
}

void SoundManager::playMusic(const string& which, bool loop)
{
    if (!musicEnabled) return;

    if (which == "menu") {
        gameMusic.stop();
        if (menuMusic.getStatus() != Music::Playing) {
            menuMusic.setLoop(loop);
            menuMusic.setVolume(musicVolume);
            menuMusic.play();
        }
    }
    else if (which == "game") {
        menuMusic.stop();
        if (gameMusic.getStatus() != Music::Playing) {
            gameMusic.setLoop(loop);
            gameMusic.setVolume(musicVolume);
            gameMusic.play();
        }
    }
}

void SoundManager::stopMusic()
{
    menuMusic.stop();
    gameMusic.stop();
}

void SoundManager::playSFX(const string& key, bool loop)
{
    if (!sfxEnabled) return;
    auto it = sounds.find(key);
    if (it != sounds.end()) {
        Sound& s = it->second;
        s.setVolume(sfxVolume);
        s.setLoop(loop);
        if (loop) {
            if (s.getStatus() != Sound::Playing) s.play();
        }
        else {
            s.stop();
            s.play();
        }
    }
}

void SoundManager::stopSFX(const string& key)
{
    auto it = sounds.find(key);
    if (it != sounds.end()) {
        it->second.stop();
    }
}

bool SoundManager::isSFXPlaying(const string& key) const
{
    auto it = sounds.find(key);
    if (it != sounds.end()) {
        return it->second.getStatus() == Sound::Playing;
    }
    return false;
}

void SoundManager::setMusicVolume(float vol)
{
    musicVolume = clamp(vol, 0.f, 100.f);
    applyVolumes();
}

void SoundManager::setSFXVolume(float vol)
{
    sfxVolume = clamp(vol, 0.f, 100.f);
    applyVolumes();
}

void SoundManager::setMusicEnabled(bool enabled)
{
    musicEnabled = enabled;
    if (!musicEnabled) stopMusic();
}

void SoundManager::setSFXEnabled(bool enabled)
{
    sfxEnabled = enabled;
}

void SoundManager::applyVolumes()
{
    menuMusic.setVolume(musicVolume);
    gameMusic.setVolume(musicVolume);
    for (auto& [k, s] : sounds) {
        s.setVolume(sfxVolume);
    }
}


