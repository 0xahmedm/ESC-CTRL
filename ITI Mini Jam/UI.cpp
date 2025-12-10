#include "UI.h"

#include <algorithm>
#include <iostream>

using namespace sf;
using namespace std;

UIButton::UIButton(const Vector2f& size, const Vector2f& pos, Color fill)
{
    rect.setSize(size);
    rect.setOrigin(size.x / 2.f, size.y / 2.f);
    rect.setPosition(pos);
    rect.setFillColor(fill);
}

bool UIButton::loadTexture(const string& path)
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

bool UIButton::contains(const Vector2i& mousePos) const
{
    return rect.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

void UIButton::draw(RenderWindow& window) const { window.draw(rect); }

Slider::Slider(float x_, float y_, float width_, int initial)
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

void Slider::setValue(int v) {
    value = clamp(v, 0, 100);
    barFill.setSize({ (width * value) / 100.f, 8.f });
    knob.setPosition(x + (width * value) / 100.f, y);
}

int Slider::getValue() const { return value; }

bool Slider::handleEvent(const RenderWindow& window, const Event& ev)
{
    Vector2i mp = Mouse::getPosition(window);
    bool changed = false;

    if (ev.type == Event::MouseButtonPressed && ev.mouseButton.button == Mouse::Left) {
        if (knob.getGlobalBounds().contains(static_cast<float>(mp.x), static_cast<float>(mp.y))) {
            dragging = true;
        }
        else if (barBg.getGlobalBounds().contains(static_cast<float>(mp.x), static_cast<float>(mp.y))) {
            float rel = clamp((mp.x - x) / width, 0.f, 1.f);
            setValue(static_cast<int>(rel * 100.f));
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
            setValue(static_cast<int>(rel * 100.f));
            changed = true;
        }
    }
    return changed;
}

void Slider::draw(RenderWindow& window) const
{
    window.draw(barBg);
    window.draw(barFill);
    window.draw(knob);
}


