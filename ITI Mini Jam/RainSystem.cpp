#include "RainSystem.h"

#include <cstdlib>
#include <ctime>

using namespace sf;
using namespace std;

RainSystem::RainSystem(int count, float W, float H)
    : WIDTH(W), HEIGHT(H)
{
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < count; i++) {
        RainDrop rd;
        rd.shape.setRadius(2.5f);
        rd.shape.setFillColor(Color(173, 216, 230, 180));
        rd.shape.setPosition(rand() % static_cast<int>(WIDTH + 500), rand() % static_cast<int>(HEIGHT));
        rd.speed = 300 + rand() % 200;
        drops.push_back(rd);
    }
}

void RainSystem::update(float dt)
{
    for (auto& rd : drops)
    {
        rd.shape.move(rd.speed / 2.f * -dt, rd.speed * dt);
        if (rd.shape.getPosition().y > HEIGHT)
            rd.shape.setPosition(rand() % static_cast<int>(WIDTH + 500), -10.f);
    }
}

void RainSystem::draw(RenderWindow& window)
{
    for (auto& rd : drops)
        window.draw(rd.shape);
}


