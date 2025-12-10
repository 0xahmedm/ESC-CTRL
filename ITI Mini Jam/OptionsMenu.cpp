#include "OptionsMenu.h"

#include <iostream>
#include <string>

using namespace sf;
using namespace std;

OptionsMenu::OptionsMenu(float WIDTH, float HEIGHT, SoundManager* sm)
{
    soundMgr = sm;

    float centerX = WIDTH / 2.f;
    float baseY = HEIGHT / 2.f - 120.f;

    musicSlider = Slider(centerX - 200.f, baseY + 20.f, 400.f, static_cast<int>(sm->musicVolume));
    sfxSlider = Slider(centerX - 200.f, baseY + 140.f, 400.f, static_cast<int>(sm->sfxVolume));

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

int OptionsMenu::update(RenderWindow& window, const Event& ev)
{
    bool changed = false;
    if (musicSlider.handleEvent(window, ev)) {
        changed = true;
        if (soundMgr) soundMgr->setMusicVolume(static_cast<float>(musicSlider.getValue()));
    }
    if (sfxSlider.handleEvent(window, ev)) {
        changed = true;
        if (soundMgr) soundMgr->setSFXVolume(static_cast<float>(sfxSlider.getValue()));
    }

    if (changed) updateValueTexts();

    if (ev.type == Event::MouseButtonPressed && ev.mouseButton.button == Mouse::Left) {
        Vector2i mp = Mouse::getPosition(window);
        if (backButton.contains(mp)) {
            if (soundMgr) soundMgr->playSFX("button_click");
            return 1;
        }
    }

    return 0;
}

void OptionsMenu::updateValueTexts() {
    musicValueText.setString(to_string(musicSlider.getValue()) + "%");
    sfxValueText.setString(to_string(sfxSlider.getValue()) + "%");
}

void OptionsMenu::draw(RenderWindow& window) {
    window.draw(titleText);
    window.draw(musicLabel);
    window.draw(sfxLabel);
    musicSlider.draw(window);
    sfxSlider.draw(window);
    window.draw(musicValueText);
    window.draw(sfxValueText);
    backButton.draw(window);
}


