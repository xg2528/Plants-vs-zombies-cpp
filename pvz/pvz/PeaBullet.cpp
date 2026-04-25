#include "pch.h"
#include "Bullet.h"
#include <cstdio>

extern void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);

static std::wstring buildPath(int index) {
    // 注意：实际路径请根据您的文件存放位置修改
    std::wstring path = L"resource/images/extra/pb00/frame_";
    if (index < 10)      path += L"000" + std::to_wstring(index);
    else if (index < 100) path += L"00" + std::to_wstring(index);
    else                 path += L"0" + std::to_wstring(index);
    path += L".png";
    return path;
}

PeaBullet::PeaBullet(float worldX, float worldY, int row, int damage)
    : Bullet(worldX, worldY, row, damage, 300.0f)  // 速度 300 像素/秒
{
    loadImg();
}

void PeaBullet::loadImg() {
    // 假设只有一帧，加载 frame_0001.png
    std::wstring path = buildPath(1);
    if (loadimage(&img, path.c_str()) != 0) {
        printf("Failed to load pea bullet image: %S\n", path.c_str());
    }
}

