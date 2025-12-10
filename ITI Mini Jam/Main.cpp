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

    RenderWindow window(mode, "ESC CTRL", Style::Fullscreen);
    window.setFramerateLimit(60);

    SoundManager soundMgr;
    soundMgr.playMusic("menu", true);

    RainSystem rain(80, WIDTH, HEIGHT);
    Menu menu(WIDTH, HEIGHT, &soundMgr);
    OptionsMenu options(WIDTH, HEIGHT, &soundMgr);
    Game game(WIDTH, HEIGHT, &soundMgr);
    GameOverScreen gameOver(WIDTH, HEIGHT);

    enum GameState { MENU_STATE, PLAYING_STATE, OPTIONS_STATE, GAMEOVER_STATE };
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
            else if (gameState == GAMEOVER_STATE) {
                window.setView(game.getCamera()); // ensure input matches camera space
                if (gameOver.update(window, e)) {
                    game.reset();
                    gameState = PLAYING_STATE;
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
            bool died = game.update(dt);
            game.draw(window);
            if (died) {
                gameState = GAMEOVER_STATE;
            }
        }
        else if (gameState == OPTIONS_STATE)
        {
            RectangleShape darkBg(Vector2f(WIDTH, HEIGHT));
            Texture bgTex = menu.tMenuBg;
            darkBg.setTexture(&bgTex);
            window.draw(darkBg);

            options.draw(window);
        }
        else if (gameState == GAMEOVER_STATE)
        {
            window.setView(game.getCamera());
            game.draw(window);
            gameOver.draw(window, game.getCamera());
        }

        window.display();
    }

    return 0;
}
