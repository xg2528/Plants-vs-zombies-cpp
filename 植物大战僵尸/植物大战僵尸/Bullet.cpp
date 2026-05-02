#include "pch.h"
#include "Bullet.h"
#include <cmath>

extern void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);

Bullet::Bullet(float worldX, float worldY, int row, int damage, float speed)
    : worldX(worldX), worldY(worldY), row(row), damage(damage), speed(speed),
    active(true), canHit(false), lifeTimer(0.05f)   // 0.05 秒无敌时间
{
}

bool Bullet::update(float delta) {
    if (!active) return false;
    // 移动
    worldX += speed * delta;
    // 出生保护计时
    if (lifeTimer > 0.0f) {
        lifeTimer -= delta;
        if (lifeTimer <= 0.0f) {
            canHit = true;
        }
    }
    // 超出边界（例如世界坐标 > 1600）则失效
    if (worldX > 1600.0f) {
        active = false;
    }
    return active;
}

void Bullet::draw(int cameraX) const {
    if (!active) return;
    int screenX = (int)(worldX - cameraX);
    int screenY = (int)worldY;
    // 如果图片有效则绘制，否则绘制一个矩形调试
    if (img.getwidth() > 0 && img.getheight() > 0) {
        putimagePng(screenX, screenY-22, &img, 0, 0, img.getwidth(), img.getheight());//减22对齐
    }
   
}