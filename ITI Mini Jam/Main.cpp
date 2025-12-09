#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;

/* ---------------------------------------------------
      PLATFORM CLASS
--------------------------------------------------- */
class Platform
{
public:
    RectangleShape body;

    Platform(float x, float y, float width, float height, Color color)
    {
        body.setSize({ width, height });
        body.setPosition(x, y);
        body.setFillColor(color);
    }

    void draw(RenderWindow& win)
    {
        win.draw(body);
    }
};

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
    Texture tIdle, tRun, tJump;

    Sprite sprite;
    RectangleShape hitbox;

    int frameW = 1024;
    int frameH = 1024;

    int framesIdle = 3;
    int framesRun = 6;
    int framesJump = 6;

    int currentState = 0;

    enum State { IDLE, RUN, JUMP };

    float timeSince = 0;
    float animSpeed = 0.09f;
    int currentFrame = 0;
    int maxFrames = 6;

    bool facingRight = true;
    bool onGround = false;

    float speed = 5.f;
    float gravity = 0.6f;
    float velY = 0.f;

    Clock animClock;

    float spriteScale = 0.2f;

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
        sprite.setScale(spriteScale, spriteScale);
        sprite.setOrigin(frameW / 2.f, frameH / 2.f);

        hitbox.setSize({ 40.f, 150.f });
        hitbox.setOrigin(20, 40);
        hitbox.setPosition(300, 300);
        hitbox.setFillColor(Color::Transparent);

        maxFrames = framesIdle;
    }

    void updateMovement()
    {
        bool moving = false;

        if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
        {
            hitbox.move(-speed, 0);
            facingRight = false;
            moving = true;
        }

        if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
        {
            hitbox.move(speed, 0);
            facingRight = true;
            moving = true;
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

        if (currentState == IDLE)
            animSpeed = 0.19f;
        else if (currentState == RUN)
            animSpeed = 0.09f;
        else
            animSpeed = 0.09f;

        if (timeSince >= animSpeed)
        {
            timeSince = 0;
            currentFrame++;
            if (currentFrame >= maxFrames) currentFrame = 0;
        }

        sprite.setTextureRect(IntRect(currentFrame * frameW, 0, frameW, frameH));

        if (facingRight)
            sprite.setScale(spriteScale, spriteScale);
        else
            sprite.setScale(-spriteScale, spriteScale);

        sprite.setPosition(hitbox.getPosition());
    }

    void platformCollision(const Platform& p)
    {
        FloatRect hb = hitbox.getGlobalBounds();
        FloatRect pb = p.body.getGlobalBounds();

        if (!hb.intersects(pb)) return;

        float hbLeft = hb.left;
        float hbRight = hb.left + hb.width;
        float hbTop = hb.top;
        float hbBottom = hb.top + hb.height;

        float pbLeft = pb.left;
        float pbRight = pb.left + pb.width;
        float pbTop = pb.top;
        float pbBottom = pb.top + pb.height;

        float overlapLeft = hbRight - pbLeft;
        float overlapRight = pbRight - hbLeft;
        float overlapTop = hbBottom - pbTop;
        float overlapBottom = pbBottom - hbTop;

        float minOverlapX = min(overlapLeft, overlapRight);
        float minOverlapY = min(overlapTop, overlapBottom);

        if (minOverlapX < minOverlapY) {
            if (overlapLeft < overlapRight)
                hitbox.move(-overlapLeft, 0);
            else
                hitbox.move(overlapRight, 0);
        }
        else {
            if (overlapTop < overlapBottom) {
                hitbox.move(0, -overlapTop);
                velY = 0;
                onGround = true;
            }
            else {
                hitbox.move(0, overlapBottom);
                velY = 0;
            }
        }
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

    View camera;
    camera.setSize(WIDTH, HEIGHT);
    camera.setCenter(WIDTH / 2, HEIGHT / 2);

    float WORLD_LEFT = 0;
    float WORLD_RIGHT = WIDTH * 10000;

    // --- Player ---
    Player player;

    vector<float> speeds = { 0, 25 , 60, 110 , 120, 130, 140 };
    ParallaxBackground bg(7, WIDTH * 10000, HEIGHT, speeds);

    Platform ground(50, HEIGHT - 200, WORLD_RIGHT - 100, 200, Color(0, 0, 0, 0));

    vector<Platform> platforms;
    platforms.emplace_back(800, HEIGHT - 250, 300, 40, Color(50, 50, 50));
    platforms.emplace_back(1400, HEIGHT - 350, 250, 40, Color(50, 50, 50));
    platforms.emplace_back(2000, HEIGHT - 200, 400, 40, Color(50, 50, 50));

    Clock dtClock;

    // --- MAIN MENU SETUP ---
    enum GameState { MENU, PLAYING,OPTIONS };
    GameState gameState = MENU;

    // Menu background
    Texture tMenuBg;
    if (!tMenuBg.loadFromFile("Assets/MenusBackgrounds/MainMenu.png"))
        cerr << "Error loading menu background!" << endl;
    Sprite menuBg(tMenuBg);
    menuBg.setScale(
        WIDTH / tMenuBg.getSize().x,
        HEIGHT / tMenuBg.getSize().y
    );

    // Button textures
    Texture tStart, tStartHover, tOptions, tOptionsHover, tExit, tExitHover;
    if (!tStart.loadFromFile("Assets/Buttons/start.png") ||
        !tStartHover.loadFromFile("Assets/Buttons/start_hover.png") ||
        !tOptions.loadFromFile("Assets/Buttons/options.png") ||
        !tOptionsHover.loadFromFile("Assets/Buttons/options_hover.png") ||
        !tExit.loadFromFile("Assets/Buttons/exit.png") ||
        !tExitHover.loadFromFile("Assets/Buttons/exit_hover.png"))
    {
        cerr << "Error loading button textures!" << endl;
    }

    Sprite btnStart(tStart), btnOptions(tOptions), btnExit(tExit);
    btnStart.setOrigin(tStart.getSize().x / 2.f, tStart.getSize().y / 2.f);
    btnOptions.setOrigin(tOptions.getSize().x / 2.f, tOptions.getSize().y / 2.f);
    btnExit.setOrigin(tExit.getSize().x / 2.f, tExit.getSize().y / 2.f);

    btnStart.setPosition((WIDTH / 2) + 350, ((HEIGHT / 2) + 150) - 150);
    btnOptions.setPosition((WIDTH / 2) + 350, (HEIGHT / 2) + 150);
    btnExit.setPosition((WIDTH / 2) + 350, ((HEIGHT / 2) + 150) + 150);

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

        window.clear();

        if (gameState == MENU)
        {
            Vector2i mousePos = Mouse::getPosition(window);

            // Draw menu background
            window.draw(menuBg);

            btnExit.setScale(0.8f, 0.8f);
            btnOptions.setScale(0.8f, 0.8f);
            btnStart.setScale(0.8f, 0.8f);


            // Update hover states
            btnStart.setTexture(btnStart.getGlobalBounds().contains(mousePos.x, mousePos.y) ? tStartHover : tStart);
            btnOptions.setTexture(btnOptions.getGlobalBounds().contains(mousePos.x, mousePos.y) ? tOptionsHover : tOptions);
            btnExit.setTexture(btnExit.getGlobalBounds().contains(mousePos.x, mousePos.y) ? tExitHover : tExit);

            // Draw buttons
            window.draw(btnStart);
            window.draw(btnOptions);
            window.draw(btnExit);

            // Mouse click actions
            if (Mouse::isButtonPressed(Mouse::Left))
            {
                if (btnStart.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    gameState = PLAYING;

                if (btnExit.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    window.close();

                if (btnOptions.getGlobalBounds().contains(mousePos.x, mousePos.y))
					gameState = OPTIONS;
            }
        }
        else if (gameState == PLAYING)
        {
            player.updateMovement();
            player.onGround = false;

            player.platformCollision(ground);
            for (auto& p : platforms)
                player.platformCollision(p);

            player.updateAnimation();

            float direction = 0;
            if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) direction = -1;
            else if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) direction = 1;

            if (direction != 0) bg.update(dt, direction, 2, bg.layerCount);
            bg.update(dt, -1, 0, 2);

            float px = player.hitbox.getPosition().x;
            px = max(px, WORLD_LEFT + WIDTH / 2);
            px = min(px, WORLD_RIGHT - WIDTH / 2);
            camera.setCenter(px, HEIGHT / 2);
            window.setView(camera);

            bg.draw(window);

            ground.draw(window);
            for (auto& p : platforms) p.draw(window);

            player.draw(window);
        }

        window.display();
    }

    return 0;
}
