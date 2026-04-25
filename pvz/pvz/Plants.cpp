#include"pch.h"
#include"Plants.h"
#include"Spawn.h"
#include"Constants.h"
extern void  putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);
void Peashooter::loadImgs()
{
    Img.clear();
    for (int i = 1; i <= m_pngNum; ++i)
    {
        IMAGE image;
        std::wstring wpath;
        // 基础路径（宽字符串）
        wpath = L"resource/images/Plants/Peashooter/frame_";
        // 根据数字补零
        if (i < 10)
            wpath += L"000" + std::to_wstring(i);
        else if (i < 100)
            wpath += L"00" + std::to_wstring(i);
        else
            wpath += L"0" + std::to_wstring(i); // 一般不超过 99 帧，但保留
        wpath += L".png";

        // 加载图片（宽字符版本）
        if (loadimage(&image, wpath.c_str()) == 0) {//等于0是没有错误
            // 可选：输出窄字符便于调试（需要转换）
            printf("success load\n");
        }
        else {
            printf("Failed to load: %S\n", wpath.c_str());
        }
        Img.push_back(image);
    }
}
Peashooter::Peashooter(int r, int c) : Plants(12) {
    row = r;
    col = c;
    type = 0;               // 豌豆射手
    shootTimer = 0.0f;
    shootInterval = 1.8f;   // 每1.8秒发射一次
    animInterval = 0.1f; 
    currentFrame = 0;
    animTimer = 0.0f;
    loadImgs();
}

void Peashooter::update(float delta) {
    
    // 动画更新：累积时间，达到间隔才切换帧
    animTimer += delta;
    if (animTimer >= animInterval) {
        animTimer -= animInterval;   // 减去间隔，保留余量
        currentFrame = (currentFrame + 1) % m_pngNum;
    }
}

void Peashooter::draw(int x, int y) {
    if (!Img.empty()) {
        putimagePng(x, y, &Img[currentFrame], 0, 0, Img[currentFrame].getwidth(), Img[currentFrame].getheight());
    }
}

void Peashooter::attack(float delta, const std::vector<std::unique_ptr<Zombies>>& zombies)
{
    float plantWorldX = GRID_WORLD_X + col * CELL_W;
    bool isAttacking=false;
    for (auto& zombie : zombies)
    {
        if (zombie->row == row && !zombie->isDead() && zombie->worldX > plantWorldX) {  // 加上死亡过滤
            isAttacking = true;
            break;
        }
    }
    // 攻击冷却
    if (isAttacking)
    {
        if (shootTimer > 0) shootTimer -= delta;
        else {
           
            printf("Peashooter at (%d,%d) shoots!\n", row, col);
            // 添加：实际创建子弹
            extern std::vector<std::unique_ptr<Bullet>> g_bullets;  // 引用全局子弹容器
            // 计算子弹起始世界坐标
            float bulletX = GRID_WORLD_X + col * CELL_W + 40;   // 植物右侧偏移
            float bulletY = GRID_WORLD_Y + row * CELL_H + 20;   // 垂直居中
            g_bullets.push_back(std::make_unique<PeaBullet>(bulletX, bulletY, row, 30));
            shootTimer = shootInterval;
        }
    }
}