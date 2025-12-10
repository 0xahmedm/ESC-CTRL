#pragma once

#include "Platform.h"
#include "Player.h"

#include <vector>

class CollisionManager {
public:
    static void resolveWithPlatform(Player& player, const Platform& platform, float& playerVelY, bool& playerOnGround);
    static void resolveAll(Player& player, std::vector<Platform>& platforms, Platform& ground, float& velY, bool& onGround);
};


