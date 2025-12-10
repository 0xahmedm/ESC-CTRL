#include "Menu.h"

#include <iostream>

using namespace sf;
using namespace std;

Menu::Menu(float WIDTH, float HEIGHT, SoundManager* sm)
{
    soundMgr = sm;
    if (!tMenuBg.loadFromFile("Assets/MenusBackgrounds/MainMenu.png"))
        cerr << "Warning: MainMenu.png not found\n";
    else {
        bg.setTexture(tMenuBg);
        bg.setScale(WIDTH / tMenuBg.getSize().x, HEIGHT / tMenuBg.getSize().y);
    }

    Vector2f btnSize(300, 80);
    btnStart = UIButton(btnSize, { (WIDTH / 2.f) + 350.f, HEIGHT / 2.f });
    btnOptions = UIButton(btnSize, { (WIDTH / 2.f) + 350.f, (HEIGHT / 2.f) + 150.f });
    btnExit = UIButton(btnSize, { (WIDTH / 2.f) + 350.f, (HEIGHT / 2.f) + 300.f });

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

int Menu::update(RenderWindow& window)
{
    Vector2i mousePos = Mouse::getPosition(window);

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

void Menu::draw(RenderWindow& window)
{
    if (tMenuBg.getSize().x) window.draw(bg);
    btnStart.draw(window);
    btnOptions.draw(window);
    btnExit.draw(window);
}


