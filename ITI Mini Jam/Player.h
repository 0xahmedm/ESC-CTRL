#pragma once

#include <SFML/Graphics.hpp>

class SoundManager;

class Player
{
public:
    sf::Texture tIdle, tRun, tJump;
    sf::Sprite sprite;
    sf::RectangleShape hitbox;
    int frameW = 1024, frameH = 1024;
    int framesIdle = 3, framesRun = 6, framesJump = 7;
    int currentState = 0;
    enum State { IDLE, RUN, JUMP };
    float timeSince = 0.f, animSpeed = 0.09f;
    int currentFrame = 0, maxFrames = 6;
    bool facingRight = true, onGround = false;
    bool movingHorizontal = false;
    float speed = 5.f, gravity = 0.6f, velY = 0.f;
    sf::Clock animClock;
    float spriteScale = 0.2f;
    SoundManager* soundMgr = nullptr;

    Player(SoundManager* manager = nullptr);

    void updateMovement();
    void updateAnimation();
    void draw(sf::RenderWindow& window);

    sf::FloatRect getGlobalBounds() const;
    sf::Vector2f getPosition() const;
    void move(float dx, float dy);
    void setPosition(float x, float y);

    bool isRunningOnGround() const;
    bool isMovingHorizontally() const;
};

