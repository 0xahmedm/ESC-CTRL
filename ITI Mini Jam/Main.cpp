#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;

/* ---------------------------------------------------
      PARALLAX BACKGROUND CLASS
--------------------------------------------------- */
class ParallaxBackground
{
public:
    int layerCount;
    vector<Texture> textures;
    vector<RectangleShape> layers;
    vector<float> speeds;
    vector<float> offsets;

    float WIDTH, HEIGHT, texHeight;

    ParallaxBackground(int count, float W, float H, const vector<float>& speedList)
        : layerCount(count), WIDTH(W), HEIGHT(H), speeds(speedList)
    {
        textures.resize(layerCount);
        layers.resize(layerCount);
        offsets.resize(layerCount, 0.f);

        for (int i = 0; i < layerCount; i++)
        {
            string filename = "Assets/Backgrounds/background_" + to_string(i + 1) + ".png";

            if (!textures[i].loadFromFile(filename))
                cerr << "Can't load " << filename << endl;

            textures[i].setRepeated(true);
            textures[i].setSmooth(true);

            texHeight = textures[i].getSize().y;

            layers[i].setSize({ WIDTH, HEIGHT });
            layers[i].setTexture(&textures[i]);
            layers[i].setTextureRect(IntRect(0, 0, WIDTH, texHeight));
        }
    }

    void update(float dt, float direction, int startLayer, int endLayer)
    {
        for (int i = startLayer; i < endLayer; i++)
        {
            offsets[i] += speeds[i] * dt * direction;
            layers[i].setTextureRect(IntRect(offsets[i], 0, WIDTH, texHeight));
        }
    }

    void draw(RenderWindow& win)
    {
        for (auto& layer : layers)
            win.draw(layer);
    }
};


/* ---------------------------------------------------
                 PLAYER CLASS
--------------------------------------------------- */

class Player
{
public:
    // Animation textures
    Texture tIdle, tRun, tJump;

    Sprite sprite;
    RectangleShape hitbox;

    int frameW = 32;
    int frameH = 32;

    int framesIdle = 11;
    int framesRun = 12;
    int framesJump = 1;

    int currentState = 0;

    enum State { IDLE, RUN, JUMP };

    float timeSince = 0;
    float animSpeed = 0.05f;
    int currentFrame = 0;
    int maxFrames = 11;

    bool facingRight = true;
    bool onGround = false;

    float speed = 2.f;
    float gravity = 0.6f;
    float velY = 0.f;

    Clock animClock;


    Player()
    {
        if (!tIdle.loadFromFile("Assets/Character/idle.png") ||
            !tRun.loadFromFile("Assets/Character/run.png") ||
            !tJump.loadFromFile("Assets/Character/jump.png"))
        {
            cout << "ERROR loading player animations!\n";
        }

        sprite.setTexture(tIdle);
        sprite.setTextureRect(IntRect(0, 0, frameW, frameH));
        sprite.setScale(3.f, 3.f);
        sprite.setOrigin(frameW / 2.f, frameH / 2.f);

        hitbox.setSize({ 40.f, 80.f });
        hitbox.setOrigin(20, 40);
        hitbox.setPosition(300, 300);
        hitbox.setFillColor(Color::Transparent);

        maxFrames = framesIdle;
    }


    void updateMovement()
    {
        bool moving = false;

        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            hitbox.move(-speed, 0);
            facingRight = false;
            moving = true;
        }

        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            hitbox.move(speed, 0);
            facingRight = true;
            moving = true;
        }

        if (Keyboard::isKeyPressed(Keyboard::Space) && onGround)
        {
            velY = -16.f;
            onGround = false;
        }

        velY += gravity;
        hitbox.move(0, velY);

        // Update animation state
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

            if (currentState == IDLE) {
                sprite.setTexture(tIdle);
                maxFrames = framesIdle;
            }
            if (currentState == RUN) {
                sprite.setTexture(tRun);
                maxFrames = framesRun;
            }
            if (currentState == JUMP) {
                sprite.setTexture(tJump);
                maxFrames = framesJump;
            }
        }
    }


    void updateAnimation()
    {
        timeSince += animClock.restart().asSeconds();

        if (timeSince >= animSpeed)
        {
            timeSince = 0;
            currentFrame++;
            if (currentFrame >= maxFrames) currentFrame = 0;
        }

        sprite.setTextureRect(IntRect(currentFrame * frameW, 0, frameW, frameH));

        if (facingRight)
            sprite.setScale(3.f, 3.f);
        else
            sprite.setScale(-3.f, 3.f);

        sprite.setPosition(hitbox.getPosition());
    }


    void groundCollision(const RectangleShape& ground)
    {
        if (hitbox.getGlobalBounds().intersects(ground.getGlobalBounds()))
        {
            float groundTop = ground.getPosition().y;
            float half = hitbox.getSize().y / 2;

            hitbox.setPosition(hitbox.getPosition().x, groundTop - half);
            velY = 0;
            onGround = true;
        }
        else onGround = false;
    }


    void draw(RenderWindow& win)
    {
        win.draw(sprite);
    }
};


/* ---------------------------------------------------
                     MAIN GAME LOOP
--------------------------------------------------- */

int main()
{
    auto mode = VideoMode::getDesktopMode();
    float WIDTH = mode.width;
    float HEIGHT = mode.height;

    RenderWindow window(mode, "ESC CTRL", Style::Fullscreen);
    window.setFramerateLimit(60);

    // Background system
    vector<float> speeds = { 0, 50, 100, 150 };
    ParallaxBackground bg(4, WIDTH, HEIGHT, speeds);

    // Player
    Player player;

    // Ground
    RectangleShape ground({ WIDTH - 100, 100 });
    ground.setPosition(50, HEIGHT - 100);
    ground.setFillColor(Color(50, 50, 50));

    Clock dtClock;

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed ||
                (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape))
                window.close();
        }

        float dt = dtClock.restart().asSeconds();

        // Player movement
        player.updateMovement();
        player.groundCollision(ground);
        player.updateAnimation();

        // Parallax scrolling
        float direction = 0;
        if (Keyboard::isKeyPressed(Keyboard::A)) direction = -1;
        else if (Keyboard::isKeyPressed(Keyboard::D)) direction = 1;

        if (direction != 0)
            bg.update(dt, direction, 2, bg.layerCount);

        bg.update(dt, -1, 0, 2);

        // Render
        window.clear();
        bg.draw(window);
        window.draw(ground);
        player.draw(window);
        window.display();
    }

    return 0;
}
