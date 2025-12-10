#include "Platform.h"

using namespace sf;

Platform::Platform(float x, float y, float width, float height, Color color)
{
    body.setSize({ width, height });
    body.setPosition(x, y);
    body.setFillColor(color);
}

void Platform::draw(RenderWindow& window)
{
    window.draw(body);
}

FloatRect Platform::getBounds() const
{
    return body.getGlobalBounds();
}


