#pragma once

#include <SFML/Graphics.hpp>

class Platform
{
public:
    sf::RectangleShape body;

    Platform(float x = 0.f, float y = 0.f, float width = 100.f, float height = 20.f, sf::Color color = sf::Color::White);

    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
};


