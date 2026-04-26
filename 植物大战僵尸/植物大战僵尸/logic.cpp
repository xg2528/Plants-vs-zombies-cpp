#include "logic.h"
#include "pch.h"

extern void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg,
    int srcX, int srcY, int srcW, int srcH);

// 静态函数：获取所有阳光动画帧（只加载一次，所有阳光共享）
static const std::vector<IMAGE>& getSunFrames() {
    static std::vector<IMAGE> frames;
    if (frames.empty()) {
        for (int i = 1; i <= 29; ++i) {
            IMAGE img;
            std::wstring wpath = L"resource/images/extra/sun/frame_";
            if (i < 10)
                wpath += L"000" + std::to_wstring(i);
            else if (i < 100)
                wpath += L"00" + std::to_wstring(i);
            else
                wpath += L"0" + std::to_wstring(i);
            wpath += L".png";
            loadimage(&img, wpath.c_str());
            frames.push_back(img);
        }
    }
    return frames;
}

// 构造函数：随机位置，绑定动画帧
Sun::Sun() {
    // 只初始化一次随机数种子
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned)time(nullptr));
        seeded = true;
    }
    // 草坪世界坐标范围 X:252~999, Y:100~500（可自行调整）
    worldX = rand() % (999 - 252 + 1) + 252;
    worldY = rand() % 400 + 100;
    currentFrame = 0;
    animTimer = 0.0f;
}

// 更新动画（基于时间差）
void Sun::update(float delta) {
    const float interval = 0.1f; // 每0.1秒切换一帧
    animTimer += delta;
    if (animTimer >= interval) {
        animTimer -= interval;
        const auto& frames = getSunFrames();
        currentFrame = (currentFrame + 1) % (int)frames.size();
    }
}

// 绘制阳光（根据摄像机偏移计算屏幕坐标）
void Sun::draw(int cameraX) const {
    const auto& frames = getSunFrames();
    if (frames.empty()) return;
    int screenX = worldX - cameraX;
    int screenY = worldY;
    const IMAGE& img = frames[currentFrame];
    putimagePng(screenX, screenY, &img, 0, 0, img.getwidth(), img.getheight());
}