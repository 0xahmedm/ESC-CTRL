#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

struct RainDrop {
    sf::CircleShape shape;
    float speed;
};

class RainSystem {
public:
    std::vector<RainDrop> drops;
    float WIDTH, HEIGHT;

    RainSystem(int count, float W, float H);

    void update(float dt);
    void draw(sf::RenderWindow& window);
};


