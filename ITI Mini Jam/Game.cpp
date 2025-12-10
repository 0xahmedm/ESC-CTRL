#include "Game.h"

#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace sf;
using namespace std;

Game::Game(float W, float H, SoundManager* sm)
    : WIDTH(W), HEIGHT(H),
    bg(5, W * 10000.f, H, { 0.f, 25.f , 60.f, 110.f , 120.f}, 0),
	BGground(1, W * 10000.f, H, { 0.f }, 5),
    ground(50.f, H - 200.f, W * 10000.f - 100.f, 200.f, Color(0, 0, 0, 0))
{
    soundMgr = sm;
    player.soundMgr = sm;
    WORLD_RIGHT = WIDTH * 10000.f;

    camera.setSize(WIDTH, HEIGHT);
    camera.setCenter(WIDTH / 2.f, HEIGHT / 2.f);

    platforms.emplace_back(800.f, HEIGHT - 250.f, 300.f, 40.f, Color(50, 50, 50));
    platforms.emplace_back(1400.f, HEIGHT - 350.f, 250.f, 40.f, Color(50, 50, 50));
    platforms.emplace_back(2000.f, HEIGHT - 200.f, 400.f, 40.f, Color(50, 50, 50));

    obstacles.emplace_back(1100.f, HEIGHT - 220.f, 90.f, 140.f);
    obstacles.emplace_back(1750.f, HEIGHT - 230.f, 90.f, 140.f);
    obstacles.emplace_back(2400.f, HEIGHT - 220.f, 90.f, 140.f);

    Texture tex;
    if (tex.loadFromFile("Assets/Props/Leaves1.png")) propTextures.push_back(tex);

    if (tex.loadFromFile("Assets/Props/Tree.png")) propTextures.push_back(tex);

    srand((unsigned)(time(0)));
    int numProps = 10000; 
    
    for (int i = 0; i < numProps; i++) {
        Sprite s;
        int randomNumberToCReateBushesAndTrees = rand() % propTextures.size();
        s.setTexture(propTextures[randomNumberToCReateBushesAndTrees]);

        float x = 100.f + (float)(rand()) / RAND_MAX * (WIDTH * 10000.f - 200.f);
        float y = 0;
        if(randomNumberToCReateBushesAndTrees == 0){
            float groundTop = HEIGHT - 200;
            y = groundTop - s.getTexture()->getSize().y * 0.4f; 
            s.setScale(0.4f, 0.4f); 
            s.setPosition(x, y);
            leavesProp.push_back(s);
        }
        else {
			float groundTop = HEIGHT - 50;
            y = groundTop - s.getTexture()->getSize().y;
			s.setScale(1.f, 1.f); 
            s.setPosition(x, y);
            treesProp.push_back(s);
        }

    }
}

bool Game::update(float dt)
{
    player.updateMovement();
    player.onGround = false;

    CollisionManager::resolveAll(player, platforms, ground, player.velY, player.onGround);

    player.updateAnimation();
    syncRunSound();

    if (checkObstacleCollision()) {
        return true;
    }

    float direction = 0;
    if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) direction = -1;
    else if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) direction = 1;

    if (direction != 0) bg.update(dt, direction, 3, bg.layerCount);
    bg.update(dt, -1, 2, 3);

    float px = player.getPosition().x;
    px = max(px, WORLD_LEFT + WIDTH / 2.f);
    px = min(px, WORLD_RIGHT - WIDTH / 2.f);
    camera.setCenter(px, HEIGHT / 2.f);

    return false;
}

void Game::draw(RenderWindow& window)
{
    window.setView(camera);
    bg.draw(window);  

    ground.draw(window);   

    for (auto& p : treesProp)
        window.draw(p);
    BGground.draw(window);


    for (auto& plat : platforms) plat.draw(window);
    for (auto& o : obstacles) o.draw(window);


    for (auto& p : leavesProp)
        window.draw(p);
    player.draw(window);
}

void Game::syncRunSound()
{
    if (!soundMgr) return;
    bool running = player.isRunningOnGround();
    if (running) {
        soundMgr->playSFX("run", true);
    }
    else {
        soundMgr->stopSFX("run");
    }
}

bool Game::checkObstacleCollision()
{
    FloatRect playerBounds = player.getGlobalBounds();
    for (auto& o : obstacles) {
        if (playerBounds.intersects(o.getBounds())) {
            if (soundMgr) soundMgr->stopSFX("run");
            return true;
        }
    }
    return false;
}

void Game::reset()
{
    player.hitbox.setPosition(300.f, 300.f);
    player.velY = 0.f;
    player.onGround = false;
    player.currentState = Player::IDLE;
    player.currentFrame = 0;
    player.sprite.setTexture(player.tIdle);
    player.sprite.setTextureRect(IntRect(0, 0, player.frameW, player.frameH));
    camera.setCenter(WIDTH / 2.f, HEIGHT / 2.f);
    syncRunSound();
}
