#include "Player.h"

#include "SoundManager.h"

#include <iostream>

using namespace sf;
using namespace std;

Player::Player(SoundManager* manager)
{
    soundMgr = manager;

    if (!tIdle.loadFromFile("Assets/Character/idle.png"))
        cerr << "Warning: idle.png not found (player texture placeholder)\n";
    if (!tRun.loadFromFile("Assets/Character/run.png"))
        cerr << "Warning: run.png not found (player texture placeholder)\n";
    if (!tJump.loadFromFile("Assets/Character/jump.png"))
        cerr << "Warning: jump.png not found (player texture placeholder)\n";

    sprite.setTexture(tIdle);
    sprite.setTextureRect(IntRect(0, 0, frameW, frameH));
    sprite.setScale(spriteScale, spriteScale);
    sprite.setOrigin(frameW / 2.f, frameH / 2.f);

    hitbox.setSize({ 40.f, 150.f });
    hitbox.setOrigin(20.f, 40.f);
    hitbox.setPosition(300.f, 300.f);
    hitbox.setFillColor(Color::Transparent);

    maxFrames = framesIdle;
}

void Player::updateMovement()
{
    bool moving = false;
    movingHorizontal = false;

    if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
    {
        hitbox.move(-speed, 0);
        facingRight = false;
        moving = true;
        movingHorizontal = true;
    }

    if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
    {
        hitbox.move(speed, 0);
        facingRight = true;
        moving = true;
        movingHorizontal = true;
    }

    if ((Keyboard::isKeyPressed(Keyboard::Space) || Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) && onGround)
    {
        velY = -16.f;
        onGround = false;
    }

    velY += gravity;
    hitbox.move(0, velY);

    int newState = currentState;
    if (!onGround)
        newState = JUMP;
    else if (moving)
        newState = RUN;
    else
        newState = IDLE;

    if (newState != currentState)
    {
        currentState = newState;
        currentFrame = 0;

        if (currentState == IDLE) { sprite.setTexture(tIdle); maxFrames = framesIdle; }
        if (currentState == RUN) { sprite.setTexture(tRun); maxFrames = framesRun; }
        if (currentState == JUMP) { sprite.setTexture(tJump); maxFrames = framesJump; }
    }
}

void Player::updateAnimation()
{
    timeSince += animClock.restart().asSeconds();

    if (currentState == IDLE) animSpeed = 0.19f;
    else if (currentState == RUN) animSpeed = 0.09f;
    else animSpeed = 0.2f; 

    if (timeSince >= animSpeed)
    {
        timeSince = 0;
        currentFrame++;
        if (currentFrame >= maxFrames) currentFrame = 0;
    }

    sprite.setTextureRect(IntRect(currentFrame * frameW, 0, frameW, frameH));
    sprite.setScale(facingRight ? spriteScale : -spriteScale, spriteScale);
    sprite.setPosition(hitbox.getPosition());
}

void Player::draw(RenderWindow& window)
{
    window.draw(sprite);
}

FloatRect Player::getGlobalBounds() const { return hitbox.getGlobalBounds(); }
Vector2f Player::getPosition() const { return hitbox.getPosition(); }
void Player::move(float dx, float dy) { hitbox.move(dx, dy); }
void Player::setPosition(float x, float y) { hitbox.setPosition(x, y); sprite.setPosition(x, y); }

bool Player::isRunningOnGround() const
{
    return movingHorizontal && onGround;
}

bool Player::isMovingHorizontally() const
{
    return movingHorizontal;
}

