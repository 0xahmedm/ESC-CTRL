#include "CollisionManager.h"

#include <algorithm>

using namespace sf;
using namespace std;

void CollisionManager::resolveWithPlatform(Player& player, const Platform& platform, float& playerVelY, bool& playerOnGround)
{
    FloatRect hb = player.getGlobalBounds();
    FloatRect pb = platform.getBounds();

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
        if (overlapLeft < overlapRight)
            player.move(-overlapLeft, 0);
        else
            player.move(overlapRight, 0);
    }
    else {
        if (overlapTop < overlapBottom) {
            player.move(0, -overlapTop);
            playerVelY = 0;
            playerOnGround = true;
        }
        else {
            player.move(0, overlapBottom);
            playerVelY = 0;
        }
    }
}

void CollisionManager::resolveAll(Player& player, vector<Platform>& platforms, Platform& ground, float& velY, bool& onGround)
{
    resolveWithPlatform(player, ground, velY, onGround);
    for (auto& p : platforms)
        resolveWithPlatform(player, p, velY, onGround);
}


