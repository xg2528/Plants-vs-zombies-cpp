#include "pch.h"
#include "Plants.h"
#include "Sun.h"       // 因为需要向全局 suns 容器中添加阳光
#include "Constants.h" // 用于网格常量
extern void  putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);
extern std::vector<Sun> suns;   // 全局阳光容器（定义在 PlayingState.cpp）

// 辅助函数：构造向日葵图片路径（18张，frame_0001.png  ~ frame_0018.png）
static std::wstring buildSunflowerPath(int index) {
    std::wstring path = L"resource/images/Plants/SunFlower/frame_";
    if (index < 10)      path += L"000" + std::to_wstring(index);
    else if (index < 100) path += L"00" + std::to_wstring(index);
    else                 path += L"0" + std::to_wstring(index);
    path += L".png";
    return path;
}

// 构造函数
Sunflower::Sunflower(int r, int c) : Plants(18) {
    row = r;
    col = c;
    type = 1;                      // 1 代表向日葵
    m_hp = 300;
    produceTimer = 0.0f;
    isProducing = false;
    produceAnimTimer = 0.0f;
    currentFrame = 0;
    animTimer = 0.0f;
    animInterval = 0.1f;           // 动画切换速度（与豌豆射手一致）
    currentInterval = 10000 + (rand() % 6) * 1000;
    loadImgs();
}

// 加载动画帧（全部18帧）
void Sunflower::loadImgs() {
    Img.clear();
    for (int i = 1; i <= m_pngNum; ++i) {
        IMAGE img;
        std::wstring path = buildSunflowerPath(i);
        if (loadimage(&img, path.c_str()) != 0) {
            printf("Failed to load sunflower frame: %S\n", path.c_str());
            continue;
        }
        Img.push_back(img);
    }
}

// 更新逻辑：动画 + 生产阳光计时 + 生产动画效果
void Sunflower::update(float delta) {
    // 动画
    animTimer += delta;
    if (animTimer >= animInterval) {
        animTimer -= animInterval;
        currentFrame = (currentFrame + 1) % (int)Img.size();
    }

    // 生产计时
    produceTimer += delta * 1000.0f;
    if (produceTimer >= currentInterval && !isProducing) {
        isProducing = true;
        produceAnimTimer = 0.0f;
        // 生成阳光
        float sunX = GRID_WORLD_X + col * CELL_W + 30;
        float sunY = GRID_WORLD_Y + row * CELL_H - 20;
        suns.emplace_back(sunX, sunY, true);
        produceTimer = 0.0f;
        currentInterval = 10000 + (rand() % 6) * 1000;   // 重新随机
    }

    // 生产动画结束
    if (isProducing) {
        produceAnimTimer += delta;
        if (produceAnimTimer >= 0.5f) {   // 动画持续0.5秒
            isProducing = false;
        }
    }
}

// 绘制：根据是否在生产状态选择源Y偏移
void Sunflower::draw(int x, int y) {
    if (Img.empty()) return;
    int halfH = Img[0].getheight() / 2;   // 图片高度的一半
    int srcY = isProducing ? halfH : 0;
    const IMAGE& img = Img[currentFrame];
    putimagePng(x, y, &img, 0, srcY, img.getwidth(), halfH);
}

void Sunflower::takeDamage(int damage) {
    m_hp -= damage;
    if (m_hp < 0) m_hp = 0;
}

bool Sunflower::isDead() const {
    return m_hp <= 0;
}
void Sunflower::attack(float delta, const std::vector<std::unique_ptr<Zombies>>& zombies) {
    // 向日葵不能攻击，留空
}