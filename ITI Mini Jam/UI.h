#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class UIButton {
public:
    sf::RectangleShape rect;
    sf::Texture tex;
    bool hasTexture = false;

    UIButton() = default;
    UIButton(const sf::Vector2f& size, const sf::Vector2f& pos, sf::Color fill = sf::Color(120, 120, 120));

    bool loadTexture(const std::string& path);
    bool contains(const sf::Vector2i& mousePos) const;
    void draw(sf::RenderWindow& window) const;
};

class Slider {
public:
    sf::RectangleShape barBg;
    sf::RectangleShape barFill;
    sf::CircleShape knob;
    float x = 0.f, y = 0.f, width = 0.f;
    int value = 0;
    bool dragging = false;

    Slider() = default;
    Slider(float x_, float y_, float width_, int initial = 50);

    void setValue(int v);
    int getValue() const;
    bool handleEvent(const sf::RenderWindow& window, const sf::Event& ev);
    void draw(sf::RenderWindow& window) const;
};


