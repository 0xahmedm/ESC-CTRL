
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <cmath>

using namespace sf;
using namespace std;

/* ---------------------------------------------------
      PLATFORM CLASS
--------------------------------------------------- */
class Platform
{
public:
    RectangleShape body;

    Platform(float x = 0, float y = 0, float width = 100, float height = 20, Color color = Color::White)
    {
        body.setSize({ width, height });
        body.setPosition(x, y);
        body.setFillColor(color);
    }

    void draw(RenderWindow& win)
    {
        win.draw(body);
    }

    FloatRect getBounds() const { return body.getGlobalBounds(); }
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
                cerr << "Warning: Can't load " << filename << " (placeholder will be used)\n";

            textures[i].setRepeated(true);
            textures[i].setSmooth(true);

            texHeight = textures[i].getSize().y ? (float)textures[i].getSize().y : HEIGHT;

            layers[i].setSize({ WIDTH, HEIGHT });
            layers[i].setTexture(&textures[i]);
            layers[i].setTextureRect(IntRect(0, 0, int(WIDTH), int(texHeight)));
        }
    }

    void update(float dt, float direction, int startLayer, int endLayer)
    {
        for (int i = startLayer; i < endLayer; i++)
        {
            offsets[i] += speeds[i] * dt * direction;
            // wrap offsets to avoid large values
            if (offsets[i] > 1000000.f || offsets[i] < -1000000.f) offsets[i] = fmod(offsets[i], 1000000.f);
            layers[i].setTextureRect(IntRect(int(offsets[i]), 0, int(WIDTH), int(texHeight)));
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
    int frameW = 1024, frameH = 1024;
    int framesIdle = 3, framesRun = 6, framesJump = 6;
    int currentState = 0;
    enum State { IDLE, RUN, JUMP };
    float timeSince = 0, animSpeed = 0.09f;
    int currentFrame = 0, maxFrames = 6;
    bool facingRight = true, onGround = false;
    float speed = 5.f, gravity = 0.6f, velY = 0.f;
    Clock animClock;
    float spriteScale = 0.2f;

    Player()
    {
        // try to load; user may replace paths later
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

            if (currentState == IDLE) { sprite.setTexture(tIdle); maxFrames = framesIdle; }
            if (currentState == RUN) { sprite.setTexture(tRun); maxFrames = framesRun; }
            if (currentState == JUMP) { sprite.setTexture(tJump); maxFrames = framesJump; }
        }
    }

    void updateAnimation()
    {
        timeSince += animClock.restart().asSeconds();

        if (currentState == IDLE) animSpeed = 0.19f;
        else if (currentState == RUN) animSpeed = 0.09f;
        else animSpeed = 0.09f;

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

    void draw(RenderWindow& win)
    {
        win.draw(sprite);
    }

    FloatRect getGlobalBounds() const { return hitbox.getGlobalBounds(); }
    Vector2f getPosition() const { return hitbox.getPosition(); }
    void move(float dx, float dy) { hitbox.move(dx, dy); }
    void setPosition(float x, float y) { hitbox.setPosition(x, y); sprite.setPosition(x, y); }
};

/* ---------------------------------------------------
                RAIN SYSTEM
--------------------------------------------------- */

struct RainDrop {
    CircleShape shape;
    float speed;
};

class RainSystem {
public:
    vector<RainDrop> drops;
    float WIDTH, HEIGHT;

    RainSystem(int count, float W, float H)
        : WIDTH(W), HEIGHT(H)
    {
        srand((unsigned)time(nullptr));
        for (int i = 0; i < count; i++) {
            RainDrop rd;
            rd.shape.setRadius(2.5f);
            rd.shape.setFillColor(Color(173, 216, 230, 180));
            rd.shape.setPosition(rand() % int(WIDTH + 500), rand() % int(HEIGHT));
            rd.speed = 300 + rand() % 200;
            drops.push_back(rd);
        }
    }

    void update(float dt)
    {
        for (auto& rd : drops)
        {
            rd.shape.move(rd.speed / 2.f * -dt, rd.speed * dt);
            if (rd.shape.getPosition().y > HEIGHT)
                rd.shape.setPosition(rand() % int(WIDTH + 500), -10.f);
        }
    }

    void draw(RenderWindow& win)
    {
        for (auto& rd : drops)
            win.draw(rd.shape);
    }
};

/* ---------------------------------------------------
                SOUND MANAGER
--------------------------------------------------- */

class SoundManager {
public:
    // Music (streamed)
    Music menuMusic;
    Music gameMusic;

    // SFX buffers & players
    map<string, SoundBuffer> buffers;
    map<string, Sound> sounds;

    // volumes (0..100)
    float musicVolume = 60.f;
    float sfxVolume = 80.f;

    bool musicEnabled = true;
    bool sfxEnabled = true;

    SoundManager()
    {
        // try load some defaults (user replaces files as needed)
        if (!menuMusic.openFromFile("Assets/SFX/BackGround.mp3"))
            cerr << "Warning: menu_music.ogg not found\n";
        if (!gameMusic.openFromFile("Assets/SFX/BackGround.mp3"))
            cerr << "Warning: game_music.ogg not found\n";

        // Reserve keys for common SFX (load may fail and will be warned)
        ensureBuffer("button_click", "Assets/Audio/button_click.wav");
        ensureBuffer("jump", "Assets/Audio/jump.wav");
        ensureBuffer("run", "Assets/Audio/run.wav");
        ensureBuffer("landing", "Assets/Audio/landing.wav");
        ensureBuffer("rain", "Assets/Audio/rain_loop.wav");

        applyVolumes();
    }

    void ensureBuffer(const string& key, const string& path)
    {
        SoundBuffer buf;
        if (!buf.loadFromFile(path)) {
            cerr << "Warning: SFX " << path << " not found (key: " << key << ")\n";
        }
        buffers[key] = move(buf);
        // create sound object
        sounds[key] = Sound();
        sounds[key].setBuffer(buffers[key]);
        sounds[key].setLoop(false);
    }

    void playMusic(const string& which, bool loop = true)
    {
        if (!musicEnabled) return;

        if (which == "menu") {
            gameMusic.stop();
            if (menuMusic.getStatus() != Music::Playing) {
                menuMusic.setLoop(loop);
                menuMusic.setVolume(musicVolume);
                menuMusic.play();
            }
        }
        else if (which == "game") {
            menuMusic.stop();
            if (gameMusic.getStatus() != Music::Playing) {
                gameMusic.setLoop(loop);
                gameMusic.setVolume(musicVolume);
                gameMusic.play();
            }
        }
    }

    void stopMusic()
    {
        menuMusic.stop();
        gameMusic.stop();
    }

    void playSFX(const string& key)
    {
        if (!sfxEnabled) return;
        auto it = sounds.find(key);
        if (it != sounds.end()) {
            Sound& s = it->second;
            s.setVolume(sfxVolume);
            s.stop(); // restart
            s.play();
        }
    }

    void setMusicVolume(float vol)
    {
        musicVolume = clamp(vol, 0.f, 100.f);
        applyVolumes();
    }

    void setSFXVolume(float vol)
    {
        sfxVolume = clamp(vol, 0.f, 100.f);
        applyVolumes();
    }

    void setMusicEnabled(bool enabled)
    {
        musicEnabled = enabled;
        if (!musicEnabled) stopMusic();
    }

    void setSFXEnabled(bool enabled)
    {
        sfxEnabled = enabled;
    }

    void applyVolumes()
    {
        menuMusic.setVolume(musicVolume);
        gameMusic.setVolume(musicVolume);
        for (auto& [k, s] : sounds) {
            s.setVolume(sfxVolume);
        }
    }
};

/* ---------------------------------------------------
                COLLISION MANAGER
--------------------------------------------------- */

class CollisionManager {
public:
    // resolve collisions between player and a platform (modifies player's hitbox & velocity/onGround)
    static void resolveWithPlatform(Player& player, const Platform& p, float& playerVelY, bool& playerOnGround)
    {
        FloatRect hb = player.getGlobalBounds();
        FloatRect pb = p.getBounds();

        if (!hb.intersects(pb)) return;

        float hbLeft = hb.left, hbRight = hb.left + hb.width;
        float hbTop = hb.top, hbBottom = hb.top + hb.height;
        float pbLeft = pb.left, pbRight = pb.left + pb.width;
        float pbTop = pb.top, pbBottom = pb.top + pb.height;

        float overlapLeft = hbRight - pbLeft;
        float overlapRight = pbRight - hbLeft;
        float overlapTop = hbBottom - pbTop;
        float overlapBottom = pbBottom - hbTop;

        float minOverlapX = min(overlapLeft, overlapRight);
        float minOverlapY = min(overlapTop, overlapBottom);

        if (minOverlapX < minOverlapY) {
            // horizontal collision
            if (overlapLeft < overlapRight)
                player.move(-overlapLeft, 0);
            else
                player.move(overlapRight, 0);
        }
        else {
            // vertical collision
            if (overlapTop < overlapBottom) {
                // landed on top of platform
                player.move(0, -overlapTop);
                playerVelY = 0;
                playerOnGround = true;
            }
            else {
                // hit from below
                player.move(0, overlapBottom);
                playerVelY = 0;
            }
        }
    }

    // resolve with many platforms
    static void resolveAll(Player& player, vector<Platform>& platforms, Platform& ground, float& velY, bool& onGround)
    {
        // first ground
        resolveWithPlatform(player, ground, velY, onGround);
        // then others
        for (auto& p : platforms)
            resolveWithPlatform(player, p, velY, onGround);
    }
};

/* ---------------------------------------------------
                    UI HELPERS
--------------------------------------------------- */

// Simple Button using RectangleShape as placeholder; supports loading texture via loadFromFile
class UIButton {
public:
    RectangleShape rect;
    Texture tex;
    bool hasTexture = false;

    UIButton() {}

    UIButton(const Vector2f& size, const Vector2f& pos, Color fill = Color(120, 120, 120))
    {
        rect.setSize(size);
        rect.setOrigin(size.x / 2.f, size.y / 2.f);
        rect.setPosition(pos);
        rect.setFillColor(fill);
    }

    // attempt to load texture and apply it (if fails, keep rectangle)
    bool loadTexture(const string& path)
    {
        if (!tex.loadFromFile(path)) {
            cerr << "Warning: UIButton failed to load texture: " << path << "\n";
            hasTexture = false;
            return false;
        }
        rect.setTexture(&tex);
        hasTexture = true;
        return true;
    }

    bool contains(const Vector2i& mp) const
    {
        return rect.getGlobalBounds().contains((float)mp.x, (float)mp.y);
    }

    void draw(RenderWindow& win) const { win.draw(rect); }
};

// Slider (horizontal) with knob, value 0..100, draggable
class Slider {
public:
    RectangleShape barBg;
    RectangleShape barFill;
    CircleShape knob;
    float x, y, width;
    int value; // 0..100
    bool dragging = false;

    Slider() {}

    Slider(float x_, float y_, float width_, int initial = 50)
    {
        x = x_; y = y_; width = width_;
        value = clamp(initial, 0, 100);

        barBg.setSize({ width, 8.f });
        barBg.setOrigin(0.f, 4.f);
        barBg.setPosition(x, y);
        barBg.setFillColor(Color(80, 80, 80, 200));

        barFill.setSize({ (width * value) / 100.f, 8.f });
        barFill.setOrigin(0.f, 4.f);
        barFill.setPosition(x, y);
        barFill.setFillColor(Color(160, 160, 160, 220));

        knob.setRadius(12.f);
        knob.setOrigin(12.f, 12.f);
        knob.setPosition(x + (width * value) / 100.f, y);
        knob.setFillColor(Color(220, 220, 220, 230));
    }

    void setValue(int v) {
        value = clamp(v, 0, 100);
        barFill.setSize({ (width * value) / 100.f, 8.f });
        knob.setPosition(x + (width * value) / 100.f, y);
    }

    int getValue() const { return value; }

    // handle mouse events, return true if changed
    bool handleEvent(const RenderWindow& win, const Event& ev)
    {
        Vector2i mp = Mouse::getPosition(win);
        bool changed = false;

        if (ev.type == Event::MouseButtonPressed && ev.mouseButton.button == Mouse::Left) {
            if (knob.getGlobalBounds().contains((float)mp.x, (float)mp.y)) {
                dragging = true;
            }
            else if (barBg.getGlobalBounds().contains((float)mp.x, (float)mp.y)) {
                // click on bar sets value
                float rel = clamp((mp.x - x) / width, 0.f, 1.f);
                setValue(int(rel * 100.f));
                changed = true;
                dragging = true;
            }
        }
        else if (ev.type == Event::MouseButtonReleased && ev.mouseButton.button == Mouse::Left) {
            dragging = false;
        }
        else if (ev.type == Event::MouseMoved) {
            if (dragging) {
                float rel = clamp((mp.x - x) / width, 0.f, 1.f);
                setValue(int(rel * 100.f));
                changed = true;
            }
        }
        return changed;
    }

    void draw(RenderWindow& win) const
    {
        win.draw(barBg);
        win.draw(barFill);
        win.draw(knob);
    }
};

/* ---------------------------------------------------
                    MENU CLASS
--------------------------------------------------- */

class Menu {
public:
    Sprite bg;
    Texture tMenuBg;
    UIButton btnStart, btnOptions, btnExit;
    Texture tStart, tStartHover, tOptions, tOptionsHover, tExit, tExitHover;
    SoundManager* soundMgr = nullptr;

    Menu(float WIDTH, float HEIGHT, SoundManager* sm = nullptr)
    {
        soundMgr = sm;
        // Load background (optional)
        if (!tMenuBg.loadFromFile("Assets/MenusBackgrounds/MainMenu.png"))
            cerr << "Warning: MainMenu.png not found\n";
        else {
            bg.setTexture(tMenuBg);
            bg.setScale(WIDTH / tMenuBg.getSize().x, HEIGHT / tMenuBg.getSize().y);
        }

        // prepare buttons as rectangles (placeholders)
        Vector2f btnSize(300, 80);
        btnStart = UIButton(btnSize, { (WIDTH / 2.f) + 350.f, HEIGHT / 2.f });
        btnOptions = UIButton(btnSize, { (WIDTH / 2.f) + 350.f, (HEIGHT / 2.f) + 150.f });
        btnExit = UIButton(btnSize, { (WIDTH / 2.f) + 350.f, (HEIGHT / 2.f) + 300.f });

        // Try to load textures (optional - warnings printed if missing)
        if (!tStart.loadFromFile("Assets/Buttons/start.png")) { /*fallback*/ }
        else btnStart.rect.setTexture(&tStart);

        if (!tStartHover.loadFromFile("Assets/Buttons/start_hover.png")) { /*fallback*/ }

        if (!tOptions.loadFromFile("Assets/Buttons/options.png")) { /*fallback*/ }
        else btnOptions.rect.setTexture(&tOptions);

        if (!tOptionsHover.loadFromFile("Assets/Buttons/options_hover.png")) { /*fallback*/ }

        if (!tExit.loadFromFile("Assets/Buttons/exit.png")) { /*fallback*/ }
        else btnExit.rect.setTexture(&tExit);

        if (!tExitHover.loadFromFile("Assets/Buttons/exit_hover.png")) { /*fallback*/ }
    }

    // 0 = none, 1 = start, 2 = options, 3 = exit
    int update(RenderWindow& window)
    {
        Vector2i mousePos = Mouse::getPosition(window);

        // hover effect: if hover texture loaded, swap texture
        if (btnStart.contains(mousePos)) {
            if (tStartHover.getSize().x) btnStart.rect.setTexture(&tStartHover);
        }
        else if (tStart.getSize().x) btnStart.rect.setTexture(&tStart);

        if (btnOptions.contains(mousePos)) {
            if (tOptionsHover.getSize().x) btnOptions.rect.setTexture(&tOptionsHover);
        }
        else if (tOptions.getSize().x) btnOptions.rect.setTexture(&tOptions);

        if (btnExit.contains(mousePos)) {
            if (tExitHover.getSize().x) btnExit.rect.setTexture(&tExitHover);
        }
        else if (tExit.getSize().x) btnExit.rect.setTexture(&tExit);

        if (Mouse::isButtonPressed(Mouse::Left)) {
            if (btnStart.contains(mousePos)) {
                if (soundMgr) soundMgr->playSFX("button_click");
                return 1;
            }
            if (btnOptions.contains(mousePos)) {
                if (soundMgr) soundMgr->playSFX("button_click");
                return 2;
            }
            if (btnExit.contains(mousePos)) {
                if (soundMgr) soundMgr->playSFX("button_click");
                return 3;
            }
        }

        return 0;
    }

    void draw(RenderWindow& window)
    {
        if (tMenuBg.getSize().x) window.draw(bg);
        btnStart.draw(window);
        btnOptions.draw(window);
        btnExit.draw(window);
    }
};

/* ---------------------------------------------------
                OPTIONS MENU CLASS
--------------------------------------------------- */

class OptionsMenu {
public:
    // layout simple vertical UI
    Slider musicSlider;
    Slider sfxSlider;
    UIButton backButton;
    Font font;
    Text titleText;
    Text musicLabel;
    Text sfxLabel;
    Text musicValueText;
    Text sfxValueText;

    SoundManager* soundMgr = nullptr;

    OptionsMenu(float WIDTH, float HEIGHT, SoundManager* sm = nullptr)
    {
        soundMgr = sm;

        float centerX = WIDTH / 2.f;
        float baseY = HEIGHT / 2.f - 120.f;

        musicSlider = Slider(centerX - 200.f, baseY + 20.f, 400.f, (int)sm->musicVolume);
        sfxSlider = Slider(centerX - 200.f, baseY + 140.f, 400.f, (int)sm->sfxVolume);

        backButton = UIButton({ 200.f, 70.f }, { centerX, baseY + 300.f }, Color(150, 150, 150));

		font = Font();
        if (!font.loadFromFile("Assets/Fonts/MyFont.ttf"))
			cerr << "Warning: arial.ttf not found (using default font)\n";

        titleText.setFont(font);
        titleText.setCharacterSize(36);
        titleText.setString("Options");
        titleText.setPosition(centerX - 60.f, baseY - 80.f);
        titleText.setFillColor(Color::White);

        musicLabel.setFont(font); musicLabel.setCharacterSize(20); musicLabel.setString("Music Volume"); musicLabel.setPosition(centerX - 200.f, baseY - 10.f); musicLabel.setFillColor(Color::White);
        sfxLabel.setFont(font); sfxLabel.setCharacterSize(20); sfxLabel.setString("SFX Volume"); sfxLabel.setPosition(centerX - 200.f, baseY + 120.f); sfxLabel.setFillColor(Color::White);

        musicValueText.setFont(font); musicValueText.setCharacterSize(18); musicValueText.setPosition(centerX + 220.f, baseY + 10.f); musicValueText.setFillColor(Color::White);
        sfxValueText.setFont(font); sfxValueText.setCharacterSize(18); sfxValueText.setPosition(centerX + 220.f, baseY + 130.f); sfxValueText.setFillColor(Color::White);

        updateValueTexts();
    }

    // returns: 0 nothing, 1 back pressed
    int update(RenderWindow& window, const Event& ev)
    {
        bool changed = false;
        if (musicSlider.handleEvent(window, ev)) {
            changed = true;
            if (soundMgr) soundMgr->setMusicVolume((float)musicSlider.getValue());
        }
        if (sfxSlider.handleEvent(window, ev)) {
            changed = true;
            if (soundMgr) soundMgr->setSFXVolume((float)sfxSlider.getValue());
        }

        if (changed) updateValueTexts();

        // handle back button click
        if (ev.type == Event::MouseButtonPressed && ev.mouseButton.button == Mouse::Left) {
            Vector2i mp = Mouse::getPosition(window);
            if (backButton.contains(mp)) {
                if (soundMgr) soundMgr->playSFX("button_click");
                return 1;
            }
        }

        return 0;
    }

    void updateValueTexts() {
        musicValueText.setString(to_string(musicSlider.getValue()) + "%");
        sfxValueText.setString(to_string(sfxSlider.getValue()) + "%");
    }

    void draw(RenderWindow& window) {
        window.draw(titleText);
        window.draw(musicLabel);
        window.draw(sfxLabel);
        musicSlider.draw(window);
        sfxSlider.draw(window);
        window.draw(musicValueText);
        window.draw(sfxValueText);
        backButton.draw(window);
    }
};

/* ---------------------------------------------------
                  GAME CLASS
--------------------------------------------------- */

class Game {
public:
    Player player;
    ParallaxBackground bg;
    vector<Platform> platforms;
    Platform ground;
    View camera;

    float WIDTH, HEIGHT;
    float WORLD_LEFT = 0;
    float WORLD_RIGHT;

    SoundManager* soundMgr = nullptr;
    CollisionManager collMgr;

    Game(float W, float H, SoundManager* sm = nullptr)
        : WIDTH(W), HEIGHT(H),
        bg(7, W * 10000.f, H, { 0.f, 25.f , 60.f, 110.f , 120.f, 130.f, 140.f }),
        ground(50.f, H - 200.f, W * 10000.f - 100.f, 200.f, Color(0, 0, 0, 0))
    {
        soundMgr = sm;
        WORLD_RIGHT = WIDTH * 10000.f;

        camera.setSize(WIDTH, HEIGHT);
        camera.setCenter(WIDTH / 2.f, HEIGHT / 2.f);

        platforms.emplace_back(800.f, HEIGHT - 250.f, 300.f, 40.f, Color(50, 50, 50));
        platforms.emplace_back(1400.f, HEIGHT - 350.f, 250.f, 40.f, Color(50, 50, 50));
        platforms.emplace_back(2000.f, HEIGHT - 200.f, 400.f, 40.f, Color(50, 50, 50));
    }

    void update(float dt)
    {
        player.updateMovement();
        player.onGround = false;

        // Use collision manager to resolve collisions (this updates player's onGround and velY)
        CollisionManager::resolveAll(player, platforms, ground, player.velY, player.onGround);

        player.updateAnimation();

        float direction = 0;
        if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) direction = -1;
        else if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) direction = 1;

        if (direction != 0) bg.update(dt, direction, 2, bg.layerCount);
        bg.update(dt, -1, 0, 2);

        float px = player.getPosition().x;
        px = max(px, WORLD_LEFT + WIDTH / 2.f);
        px = min(px, WORLD_RIGHT - WIDTH / 2.f);
        camera.setCenter(px, HEIGHT / 2.f);
    }

    void draw(RenderWindow& window)
    {
        window.setView(camera);

        bg.draw(window);
        ground.draw(window);
        for (auto& p : platforms) p.draw(window);
        player.draw(window);
    }
};

/* ---------------------------------------------------
                     MAIN GAME LOOP
--------------------------------------------------- */

int main()
{
    auto mode = VideoMode::getDesktopMode();
    float WIDTH = (float)mode.width;
    float HEIGHT = (float)mode.height;

    RenderWindow window(mode, "ESC CTRL", Style::Fullscreen);
    window.setFramerateLimit(60);

    // Sound Manager (shared)
    SoundManager soundMgr;

    // Start playing menu music
    soundMgr.playMusic("menu", true);

    // Init UI & game systems
    RainSystem rain(80, WIDTH, HEIGHT);
    Menu menu(WIDTH, HEIGHT, &soundMgr);
    OptionsMenu options(WIDTH, HEIGHT, &soundMgr);
    Game game(WIDTH, HEIGHT, &soundMgr);

    enum GameState { MENU_STATE, PLAYING_STATE, OPTIONS_STATE };
    GameState gameState = MENU_STATE;

    Clock dtClock;

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed ||
                (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape))
                window.close();

            if (gameState == OPTIONS_STATE) {
                int res = options.update(window, e);
                if (res == 1) {
                    gameState = MENU_STATE;
                }
            }
        }

        float dt = dtClock.restart().asSeconds();

        window.clear();

        if (gameState == MENU_STATE)
        {
            int menuResult = menu.update(window);
            menu.draw(window);

            rain.update(dt);
            rain.draw(window);

            if (menuResult == 1) {
                gameState = PLAYING_STATE;
            }
            if (menuResult == 3) window.close();
            if (menuResult == 2) {
                gameState = OPTIONS_STATE;
            }
        }
        else if (gameState == PLAYING_STATE)
        {
            game.update(dt);
            game.draw(window);

        }
        else if (gameState == OPTIONS_STATE)
        {

            RectangleShape darkBg(Vector2f(WIDTH, HEIGHT));
			Texture bgTex = menu.tMenuBg;
            darkBg.setTexture(&bgTex);
            window.draw(darkBg);

            options.draw(window);
        }

        window.display();
    }

    return 0;
}
