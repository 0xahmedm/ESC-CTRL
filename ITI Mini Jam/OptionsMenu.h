#pragma once

#include <SFML/Graphics.hpp>

#include "SoundManager.h"
#include "UI.h"

class OptionsMenu {
public:
    Slider musicSlider;
    Slider sfxSlider;
    UIButton backButton;
    sf::Font font;
    sf::Text titleText;
    sf::Text musicLabel;
    sf::Text sfxLabel;
    sf::Text musicValueText;
    sf::Text sfxValueText;

    SoundManager* soundMgr = nullptr;

    OptionsMenu(float WIDTH, float HEIGHT, SoundManager* sm = nullptr);

    int update(sf::RenderWindow& window, const sf::Event& ev);
    void draw(sf::RenderWindow& window);

private:
    void updateValueTexts();
};


