#include"pch.h"
#include "Sun.h"

extern void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);

// 构造函数
Sun::Sun() {
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(nullptr));
        seeded = true;
    }
    // 随机位置，范围根据你的草坪调整
	worldX = (float)(rand() % 748 + 252 - 30);//微调30像素，使阳光不会完全出现在边界外
    worldY = 75;
    currentFrame = 0;
    animTimer = 0.0f;
	isSunFlower = false; // 默认不是向日葵产生的阳光
    speed = 30.0f; // 默认下降速度（像素/秒）
    drop_time = rand() % 3 + 3; // 3-5秒后落地
	dropTimer = 0.0f;
	waitTime = 6; // 默认等待时间（秒），可以根据需要调整
    waitTimer = 0.0f;
}

// 更新动画
void Sun::update(float delta) {
    const float interval = 0.1f;
    animTimer += delta;
    if (animTimer >= interval) {
        animTimer -= interval;
        const auto& frames = getFrames();
        currentFrame = (currentFrame + 1) % (int)frames.size();
    }
    // 更新阳光位置
    if (!isSunFlower) {
        worldY += speed * delta;
		dropTimer += delta;
		if (dropTimer >= drop_time) {
			dropTimer = 0.0f;
			speed = 0.0f; // 落地后停止下降
		}

    }
    if(speed == 0.0f) {
		
		waitTimer += delta;
        if (waitTimer >= waitTime) {
            waitTimer = 0.0f;
			isCollected = true; // 超时未收集，标记为已收集（即删除）
        }
	}
    
}

// 绘制阳光
void Sun::draw(int cameraX) const {
    const auto& frames = getFrames();
    if (frames.empty()) return;
    int screenX = (int)(worldX - cameraX);
    int screenY = (int)worldY;
    const IMAGE& img = frames[currentFrame];
    putimagePng(screenX, screenY, &img, 0, 0, img.getwidth(), img.getheight());
}

// 静态私有函数：加载所有阳光动画帧（只加载一次，所有阳光共享）
const std::vector<IMAGE>& Sun::getFrames() {
    static std::vector<IMAGE> frames;
    if (frames.empty()) {
        for (int i = 1; i <= 29; ++i) {
            IMAGE img;
            std::wstring wpath = L"resource/images/extra/sun/frame_";
            if (i < 10) wpath += L"000" + std::to_wstring(i);
            else if (i < 100) wpath += L"00" + std::to_wstring(i);
            else wpath += L"0" + std::to_wstring(i);
            wpath += L".png";
            loadimage(&img, wpath.c_str());
            frames.push_back(img);
        }
    }
    return frames;
}


// 伪代码修正思路：
// 1. 检查 Sun::Sun(float x, float y, bool fromSunFlower) 构造函数，确保 waitTime 和 waitTimer 均有初始化。
// 2. 若类头文件中 waitTime/waitTimer 有默认值则无需处理，否则在构造函数初始化列表或函数体中赋初值。
// 3. 保持其它成员初始化逻辑不变。

Sun::Sun(float x, float y, bool fromSunFlower) {
    worldX = x;
    worldY = y;
    isSunFlower = fromSunFlower;
    currentFrame = 0;
    animTimer = 0.0f;   
    waitTime = 6.0f;   // 明确初始化
    waitTimer = 0.0f;  // 明确初始化
    // 如果是向日葵产生的阳光，速度设为0，否则随机速度并下落
    if (isSunFlower) {
        speed = 0.0f;
        dropTimer = 0.0f;
        drop_time = 0.0f;
    }
    else {
        speed = (float)(rand() % 50 + 30);   // 随机下落速度 30-80 像素/秒
        drop_time = (float)(rand() % 3 + 2);  // 下落持续 2-4 秒
        dropTimer = 0.0f;
    }
    // 使用共享动画帧（静态 getFrames 返回已有图片）
}