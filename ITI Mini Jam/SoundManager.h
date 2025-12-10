#pragma once

#include <SFML/Audio.hpp>
#include <algorithm>
#include <map>
#include <string>

class SoundManager {
public:
    sf::Music menuMusic;
    sf::Music gameMusic;

    std::map<std::string, sf::SoundBuffer> buffers;
    std::map<std::string, sf::Sound> sounds;

    float musicVolume = 60.f;
    float sfxVolume = 80.f;

    bool musicEnabled = true;
    bool sfxEnabled = true;

    SoundManager();

    void ensureBuffer(const std::string& key, const std::string& path, bool loop = false);

    void playMusic(const std::string& which, bool loop = true);
    void stopMusic();

    void playSFX(const std::string& key, bool loop = false);
    void stopSFX(const std::string& key);
    bool isSFXPlaying(const std::string& key) const;

    void setMusicVolume(float vol);
    void setSFXVolume(float vol);
    void setMusicEnabled(bool enabled);
    void setSFXEnabled(bool enabled);

    void applyVolumes();
};


