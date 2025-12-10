#pragma once

#include <SFML/Graphics.hpp>

#include "UI.h"

class GameOverScreen
{
public:
    GameOverScreen(float width, float height);

    bool update(sf::RenderWindow& window, const sf::Event& ev);
    void draw(sf::RenderWindow& window, const sf::View& view);

private:
    sf::RectangleShape overlay;
    sf::Font font;
    sf::Text title;
    UIButton restartButton;
    sf::Text restartLabel;
};

