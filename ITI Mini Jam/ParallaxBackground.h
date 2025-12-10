#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class ParallaxBackground
{
public:
    int layerCount;
    std::vector<sf::Texture> textures;
    std::vector<sf::RectangleShape> layers;
    std::vector<float> speeds;
    std::vector<float> offsets;
    int startLayer = 0;

    float WIDTH, HEIGHT, texHeight;

    ParallaxBackground(int count, float W, float H, const std::vector<float>& speedList, int start);

    void update(float dt, float direction, int startLayer, int endLayer);

    void draw(sf::RenderWindow& window);
};


