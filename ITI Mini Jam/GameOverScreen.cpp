#include "GameOverScreen.h"

#include <iostream>

using namespace sf;
using namespace std;

GameOverScreen::GameOverScreen(float width, float height)
{
    overlay.setSize({ width, height });
    overlay.setFillColor(Color(0, 0, 0, 180));

    if (!font.loadFromFile("Assets/Fonts/MyFont.ttf"))
        cerr << "Warning: MyFont.ttf not found for GameOver screen\n";

    title.setFont(font);
    title.setCharacterSize(64);
    title.setString("Game Over");
    title.setFillColor(Color::White);
    FloatRect bounds = title.getLocalBounds();
    title.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    title.setPosition(width / 2.f, height / 2.f - 120.f);

    restartButton = UIButton({ 250.f, 80.f }, { width / 2.f, height / 2.f + 40.f }, Color(200, 200, 200, 230));

    restartLabel.setFont(font);
    restartLabel.setCharacterSize(28);
    restartLabel.setFillColor(Color::Black);
    restartLabel.setString("Restart");
    FloatRect lb = restartLabel.getLocalBounds();
    restartLabel.setOrigin(lb.width / 2.f, lb.height / 2.f);
    restartLabel.setPosition(restartButton.rect.getPosition().x, restartButton.rect.getPosition().y - 6.f);
}

bool GameOverScreen::update(RenderWindow& window, const Event& ev)
{
    if (ev.type == Event::MouseButtonPressed && ev.mouseButton.button == Mouse::Left) {
        Vector2i mpPixels = Mouse::getPosition(window);
        Vector2f mpWorld = window.mapPixelToCoords(mpPixels, window.getView());
        if (restartButton.contains(Vector2i(static_cast<int>(mpWorld.x), static_cast<int>(mpWorld.y)))) {
            return true;
        }
    }
    return false;
}

void GameOverScreen::draw(RenderWindow& window, const View& view)
{
    Vector2f size = view.getSize();
    Vector2f center = view.getCenter();

    overlay.setSize(size);
    overlay.setOrigin(size.x / 2.f, size.y / 2.f);
    overlay.setPosition(center);

    title.setPosition(center.x, center.y - 120.f);
    restartButton.rect.setPosition(center.x, center.y + 40.f);

    FloatRect lb = restartLabel.getLocalBounds();
    restartLabel.setOrigin(lb.width / 2.f, lb.height / 2.f);
    restartLabel.setPosition(restartButton.rect.getPosition().x, restartButton.rect.getPosition().y - 6.f);

    window.draw(overlay);
    window.draw(title);
    restartButton.draw(window);
    window.draw(restartLabel);
}

