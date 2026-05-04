#include "pch.h"
#include "Plants.h"

extern void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);

static void loadFrameSet(std::vector<IMAGE>& imgs, int count, const std::wstring& dir) {
    for (int i = 1; i <= count; ++i) {
        IMAGE image;
        std::wstring wpath = dir;
        if (i < 10)
            wpath += L"000" + std::to_wstring(i);
        else if (i < 100)
            wpath += L"00" + std::to_wstring(i);
        wpath += L".png";
        if (loadimage(&image, wpath.c_str()) == 0) {
            printf("WallNut frame loaded\n");
        } else {
            printf("Failed to load: %S\n", wpath.c_str());
        }
        imgs.push_back(image);
    }
}

void WallNut::loadImgs() {
    wallNut0_imgs.clear();
    wallNut1_imgs.clear();
    wallNut2_imgs.clear();
    loadFrameSet(wallNut0_imgs, 16, L"resource/images/Plants/WallNut/WallNut0/frame_");
    loadFrameSet(wallNut1_imgs, 11, L"resource/images/Plants/WallNut/WallNut1/frame_");
    loadFrameSet(wallNut2_imgs, 15, L"resource/images/Plants/WallNut/WallNut2/frame_");
    currentStage = 0;
    m_pngNum = 16;
    Img = wallNut0_imgs;
}

void WallNut::updateStage() {
    int oldStage = currentStage;
    if (m_hp > 1800) {
        currentStage = 0;
        m_pngNum = 16;
        Img = wallNut0_imgs;
    } else if (m_hp > 900) {
        currentStage = 1;
        m_pngNum = 11;
        Img = wallNut1_imgs;
    } else {
        currentStage = 2;
        m_pngNum = 15;
        Img = wallNut2_imgs;
    }
    if (currentStage != oldStage) {
        currentFrame = 0;
        animTimer = 0.0f;
    }
}

WallNut::WallNut(int r, int c) : Plants(16) {
    row = r;
    col = c;
    type = 3;
    m_hp = 3000;
    currentFrame = 0;
    animTimer = 0.0f;
    animInterval = 0.1f;
    currentStage = 0;
    loadImgs();
}

void WallNut::update(float delta) {
    updateStage();
    animTimer += delta;
    if (animTimer >= animInterval) {
        animTimer -= animInterval;
        currentFrame = (currentFrame + 1) % m_pngNum;
    }
}

void WallNut::draw(int x, int y) {
    const auto& frames = Img;
    if (!frames.empty() && currentFrame < (int)frames.size()) {
        putimagePng(x, y, &frames[currentFrame], 0, 0, frames[currentFrame].getwidth(), frames[currentFrame].getheight());
    }
}
