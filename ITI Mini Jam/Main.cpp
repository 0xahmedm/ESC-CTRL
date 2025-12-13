#include <SFML/Graphics.hpp>

#include "Game.h"
#include "Menu.h"
#include "OptionsMenu.h"
#include "RainSystem.h"
#include "GameOverScreen.h"
#include "SoundManager.h"

using namespace sf;

int main()
{
    auto mode = VideoMode::getDesktopMode();
    float WIDTH = static_cast<float>(mode.width);
    float HEIGHT = static_cast<float>(mode.height);

    // Borderless fullscreen to avoid OS white flash
    RenderWindow window(mode, "ESC CTRL", Style::None);
    window.setFramerateLimit(60);

    //  First black frame immediately
    window.clear(Color::Black);
    window.display();

    // ---- create objects after first frame ----
    SoundManager soundMgr;
    soundMgr.playMusic("menu", true);

    RainSystem rain(80, WIDTH, HEIGHT);
    Menu menu(WIDTH, HEIGHT, &soundMgr);
    OptionsMenu options(WIDTH, HEIGHT, &soundMgr);
    GameOverScreen gameOver(WIDTH, HEIGHT);

    Game* game = nullptr;

    enum GameState { MENU_STATE, PLAYING_STATE, OPTIONS_STATE, GAMEOVER_STATE };
    GameState gameState = MENU_STATE;

    Clock dtClock;

    // 🔹 Fade-in overlay
    RectangleShape fadeOverlay(Vector2f(WIDTH, HEIGHT));
    fadeOverlay.setFillColor(Color::Black);
    float fadeAlpha = 255.f;      // start fully black
    float fadeSpeed = 150.f;      // alpha decrease per second

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
                if (res == 1)
                    gameState = MENU_STATE;
            }
            else if (gameState == GAMEOVER_STATE && game) {
                window.setView(game->getCamera());
                if (gameOver.update(window, e)) {
                    game->reset();
                    gameState = PLAYING_STATE;
                }
            }
        }

        float dt = dtClock.restart().asSeconds();
        window.clear(Color::Black);

        // --- Drawing logic ---
        if (gameState == MENU_STATE)
        {
            int menuResult = menu.update(window);
            menu.draw(window);

            rain.update(dt);
            rain.draw(window);

            if (menuResult == 1) { // PLAY
                if (!game)
                    game = new Game(WIDTH, HEIGHT, &soundMgr);
                gameState = PLAYING_STATE;
            }
            else if (menuResult == 2) { // OPTIONS
                gameState = OPTIONS_STATE;
            }
            else if (menuResult == 3) { // EXIT
                window.close();
            }
        }
        else if (gameState == PLAYING_STATE && game)
        {
            bool died = game->update(dt);
            game->draw(window);
            if (died)
                gameState = GAMEOVER_STATE;
        }
        else if (gameState == OPTIONS_STATE)
        {
            RectangleShape bg(Vector2f(WIDTH, HEIGHT));
            bg.setTexture(&menu.tMenuBg);
            window.draw(bg);
            options.draw(window);
        }
        else if (gameState == GAMEOVER_STATE && game)
        {
            window.setView(game->getCamera());
            game->draw(window);
            gameOver.draw(window, game->getCamera());
        }

        // --- Fade-in overlay ---
        if (fadeAlpha > 0.f)
        {
            fadeAlpha -= fadeSpeed * dt;
            if (fadeAlpha < 0.f) fadeAlpha = 0.f;
            fadeOverlay.setFillColor(Color(0, 0, 0, static_cast<Uint8>(fadeAlpha)));
            window.draw(fadeOverlay);
        }

        window.display();
    }

    delete game;
    return 0;
}
