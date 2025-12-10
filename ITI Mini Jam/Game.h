#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "CollisionManager.h"
#include "Obstacle.h"
#include "ParallaxBackground.h"
#include "Platform.h"
#include "Player.h"
#include "SoundManager.h"


using namespace sf;

class Game {
public:
    Player player;
    ParallaxBackground bg;
    ParallaxBackground BGground;

    std::vector<Platform> platforms;
    std::vector<Obstacle> obstacles;
    Platform ground;
    sf::View camera;

    std::vector<Texture> propTextures;
    std::vector<Sprite> treesProp;
    std::vector<Sprite> leavesProp;

    float WIDTH, HEIGHT;
    float WORLD_LEFT = 0.f;
    float WORLD_RIGHT;

    SoundManager* soundMgr = nullptr;

    Game(float W, float H, SoundManager* sm = nullptr);

    // returns true if player died this frame
    bool update(float dt);
    void draw(sf::RenderWindow& window);
    void reset();
    const sf::View& getCamera() const { return camera; }

private:
    void syncRunSound();
    bool checkObstacleCollision();
};

