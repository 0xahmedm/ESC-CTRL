#include "ParallaxBackground.h"

#include <cmath>
#include <iostream>

using namespace sf;
using namespace std;

ParallaxBackground::ParallaxBackground(int count, float W, float H, const vector<float>& speedList, int start)
	: layerCount(count), WIDTH(W), HEIGHT(H), speeds(speedList), startLayer(start)
{
    textures.resize(layerCount);
    layers.resize(layerCount);
    offsets.resize(layerCount, 0.f);

    for (int i = 0; i < layerCount; i++)
    {
		int name = start + i;
        string filename = "Assets/Backgrounds/BG_0" + to_string(name) + ".png";

        if (!textures[i].loadFromFile(filename))
            cerr << "Warning: Can't load " << filename << " (placeholder will be used)\n";

        textures[i].setRepeated(true);
        textures[i].setSmooth(true);

        texHeight = textures[i].getSize().y ? static_cast<float>(textures[i].getSize().y) : HEIGHT;

        layers[i].setSize({ WIDTH, HEIGHT });
        layers[i].setTexture(&textures[i]);
        layers[i].setTextureRect(IntRect(0, 0, static_cast<int>(WIDTH), static_cast<int>(texHeight)));
    }
}

void ParallaxBackground::update(float dt, float direction, int startLayer, int endLayer)
{
    for (int i = startLayer; i < endLayer; i++)
    {
        offsets[i] += speeds[i] * dt * direction;
        if (offsets[i] > 1000000.f || offsets[i] < -1000000.f) offsets[i] = fmod(offsets[i], 1000000.f);
        layers[i].setTextureRect(IntRect(static_cast<int>(offsets[i]), 0, static_cast<int>(WIDTH), static_cast<int>(texHeight)));
    }
}

void ParallaxBackground::draw(RenderWindow& window)
{
    for (auto& layer : layers)
        window.draw(layer);
}


