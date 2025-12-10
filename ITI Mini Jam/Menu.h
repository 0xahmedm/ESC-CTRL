#pragma once

#include <SFML/Graphics.hpp>

#include "SoundManager.h"
#include "UI.h"

class Menu {
public:
    sf::Sprite bg;
    sf::Texture tMenuBg;
    UIButton btnStart, btnOptions, btnExit;
    sf::Texture tStart, tStartHover, tOptions, tOptionsHover, tExit, tExitHover;
    SoundManager* soundMgr = nullptr;

    Menu(float WIDTH, float HEIGHT, SoundManager* sm = nullptr);

    int update(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
};


