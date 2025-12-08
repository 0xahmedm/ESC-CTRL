#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;

void parallexMove(vector<sf::RectangleShape>& layers, const vector<float>& speeds, float dt, float* offsets, float WIDTH, float HEIGHT, float direction, int startIndex, int endIndex);

int main() {
    auto mode = sf::VideoMode::getDesktopMode();
    const float WIDTH = mode.width;
    const float HEIGHT = mode.height;
    int texH = 0;
    sf::RenderWindow window(mode, "ESC:CTRL", sf::Style::Fullscreen);

    const int LAYERS_COUNT = 4;
    vector<sf::Texture> layerTextures(LAYERS_COUNT);
    vector<sf::RectangleShape> layerSprites(LAYERS_COUNT);
    vector<float> speeds = { 0.f, 50.f, 100.f, 150.f };

    for (int i = 0; i < LAYERS_COUNT; ++i) {
        string filename = "Assets/background_" + to_string(i) + ".png";
        if (!layerTextures[i].loadFromFile(filename)) {
            cerr << "Can't Load File: " << filename << endl;
        }

        layerTextures[i].setRepeated(true);
        layerTextures[i].setSmooth(true);

        texH = layerTextures[i].getSize().y;

        layerSprites[i].setSize({ WIDTH, HEIGHT });
        layerSprites[i].setTexture(&layerTextures[i]);
        layerSprites[i].setTextureRect(sf::IntRect(0, 0, WIDTH, texH));
    }

    sf::Clock clock;
    float offsets[LAYERS_COUNT] = { 0 };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            {
                window.close();
            }
        }

        float dt = clock.restart().asSeconds();

        float direction = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) direction = 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) direction = -1.f;

        if (direction != 0)

            parallexMove(layerSprites, speeds, dt, offsets, WIDTH, texH, direction, 2, LAYERS_COUNT);


        parallexMove(layerSprites, speeds, dt, offsets, WIDTH, texH, -1, 0, 2);


        window.clear();
        for (int i = 0; i < LAYERS_COUNT; ++i) {
            window.draw(layerSprites[i]);
        }
        window.display();
    }

    return 0;
}

void parallexMove(vector<sf::RectangleShape>& layers, const vector<float>& speeds, float dt, float* offsets, float WIDTH, float HEIGHT, float direction, int startIndex, int endIndex) {
    for (int i = startIndex; i < endIndex; ++i) {
        offsets[i] += speeds[i] * dt * direction;
        layers[i].setTextureRect(sf::IntRect(offsets[i], 0, WIDTH, HEIGHT));
    }
}