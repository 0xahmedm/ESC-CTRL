#include "Obstacle.h"

using namespace sf;

Obstacle::Obstacle(float x, float y, float width, float height, Color color)
{
    body.setSize({ width, height });
    body.setOrigin(width / 2.f, height / 2.f);
    body.setPosition(x, y);
    body.setFillColor(color);
}

void Obstacle::draw(RenderWindow& window)
{
    window.draw(body);
}

FloatRect Obstacle::getBounds() const
{
    return body.getGlobalBounds();
}


