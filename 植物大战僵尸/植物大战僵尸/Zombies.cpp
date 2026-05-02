#include "pch.h"
#include "Zombies.h"
#include <cstdio>
#include "AudioManager.h"

extern void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);
extern AudioManager audio;

// ---------- 静态成员初始化 ----------
std::vector<IMAGE> NormalZombie::s_frames;
std::vector<IMAGE> NormalZombie::attack_frames;
int NormalZombie::s_totalFrames = 0;
int NormalZombie::s_attackTotalFrames = 0;
bool NormalZombie::s_imagesLoaded = false;

// ---------- ConeheadZombie static members ----------
std::vector<IMAGE> ConeheadZombie::s_frames;
std::vector<IMAGE> ConeheadZombie::attack_frames;
int ConeheadZombie::s_totalFrames = 0;
int ConeheadZombie::s_attackTotalFrames = 0;
bool ConeheadZombie::s_imagesLoaded = false;

// ---------- 辅助函数：构造图片路径（根据实际存放位置修改） ----------
static std::wstring buildPath(int index,std::wstring str= L"resource/images/extra/Zombie/frame_") {
    // 请根据你的实际图片路径修改下面的字符串
    std::wstring path = str;
    if (index < 10)
        path += L"000" + std::to_wstring(index);
    else if (index < 100)
        path += L"00" + std::to_wstring(index);
    else
        path += L"0" + std::to_wstring(index);
    path += L".png";
    return path;
}

// ---------- 静态方法：加载共享图片（只需调用一次） ----------
void NormalZombie::loadSharedImages() {
    if (s_imagesLoaded) return;

    s_frames.clear();
    int maxExpected = 18;   // 根据实际帧数修改
    int loaded = 0;
    for (int i = 1; i <= maxExpected; ++i) {
        std::wstring path = buildPath(i, L"resource/images/extra/Zombie/frame_");
        IMAGE img;
        if (loadimage(&img, path.c_str()) != 0) {   // 失败返回非0
            printf("Failed to load zombie frame: %S\n", path.c_str());
            continue;
        }
        s_frames.push_back(img);
        loaded++;
    }

    s_totalFrames = loaded;
    s_imagesLoaded = true;

    if (s_totalFrames == 0) {
        printf("Warning: No zombie images loaded, using rectangle fallback.\n");
    }
    else {
        printf("Successfully loaded %d zombie animation frames.\n", s_totalFrames);
    }
    loaded = 0;
    for (int i = 1; i <= 21; ++i) {
        std::wstring path = buildPath(i, L"resource/images/extra/Zombie_Attack/frame_");
        IMAGE img;
        if (loadimage(&img, path.c_str()) != 0) {   // 失败返回非0
            printf("Failed to load zombie frame: %S\n", path.c_str());
            continue;
        }
        attack_frames.push_back(img);
        loaded++;
       
    }
    s_attackTotalFrames = loaded;
}
                                               
// ---------- 构造函数：初始化实例属性（不加载图片，使用共享的静态图片） ----------
NormalZombie::NormalZombie(int r, float startX, float startY) {
    row = r;
    worldX = startX;
    worldY = startY;
    hp = 300;
    isAttacking = false;
    speed = rand() % 6 + 10;          // 随机速度 10~15 像素/秒

    // 动画状态初始化
    currentFrame = 0;
    animTimer = 0.0f;
    animInterval = 0.1f;               // 每 0.1 秒切换一帧
    attackTimer = 0.0f;
    attackInterval = 0.65f;   // 每0.65秒攻击一次
    attackDamage = 60;


    //概率播放僵尸叫声
    int porbility = rand() % 10;
    if (!porbility)
    {
        sounds();
    }
}

// ---------- 更新逻辑：移动 + 动画 ----------
void NormalZombie::update(float delta) {
    if (!isAttacking) {
        worldX -= speed * delta;   // 只有非攻击状态才移动
    }
    // 动画更新（保持不变）
    const auto& frames = isAttacking ? attack_frames : s_frames;
    int total = frames.size();
    if (total > 0) {
        animTimer += delta;
        if (animTimer >= animInterval) {
            animTimer -= animInterval;
            currentFrame = (currentFrame + 1) % total;
        }
    }
}

// ---------- 绘制逻辑：优先使用共享动画图片，否则绘制灰色矩形 ----------
void NormalZombie::draw(int screenX, int screenY) {
    // 根据攻击状态选择动画序列
    const std::vector<IMAGE>& frames = isAttacking ? attack_frames : s_frames;
    int total = (int)frames.size();

    if (total > 0 && currentFrame < total) {
        const IMAGE& img = frames[currentFrame];
        putimagePng(screenX, screenY, &img, 0, 0, img.getwidth(), img.getheight());
    }
}

// ---------- 受到伤害 ----------
void NormalZombie::takeDamage(int damage) {
    hp -= damage;
    if (hp < 0) hp = 0;
}

// ---------- 是否死亡 ----------
bool NormalZombie::isDead() const {
    return hp <= 0;
}

void NormalZombie::drawDead()
{




}
void NormalZombie::sounds()
{
    int id = rand() % 6 + 1;
    std::string ID = std::to_string(id);
    std::string file = "groan" + ID + ".mp3";
    audio.playEffect(file);
}

// ---------- ConeheadZombie: load shared animation images ----------
void ConeheadZombie::loadSharedImages() {
    if (s_imagesLoaded) return;

    s_frames.clear();
    int maxExpected = 21;
    int loaded = 0;
    for (int i = 1; i <= maxExpected; ++i) {
        std::wstring path = buildPath(i, L"resource/images/extra/ConeheadZombie/frame_");
        IMAGE img;
        if (loadimage(&img, path.c_str()) != 0) {
            printf("Failed to load ConeheadZombie frame: %S\n", path.c_str());
            continue;
        }
        s_frames.push_back(img);
        loaded++;
    }
    s_totalFrames = loaded;
    s_imagesLoaded = true;

    if (s_totalFrames == 0) {
        printf("Warning: No ConeheadZombie images loaded.\n");
    }
    else {
        printf("Successfully loaded %d ConeheadZombie walking frames.\n", s_totalFrames);
    }

    // Load attack frames
    loaded = 0;
    for (int i = 1; i <= 11; ++i) {
        std::wstring path = buildPath(i, L"resource/images/extra/ConeheadZombie_Attack/frame_");
        IMAGE img;
        if (loadimage(&img, path.c_str()) != 0) {
            printf("Failed to load ConeheadZombie attack frame: %S\n", path.c_str());
            continue;
        }
        attack_frames.push_back(img);
        loaded++;
    }
    s_attackTotalFrames = loaded;
    printf("Successfully loaded %d ConeheadZombie attack frames.\n", s_attackTotalFrames);
}

// ---------- ConeheadZombie constructor ----------
ConeheadZombie::ConeheadZombie(int r, float startX, float startY) {
    row = r;
    worldX = startX;
    worldY = startY;
    hp = 660;
    isAttacking = false;
    speed = rand() % 6 + 10;

    currentFrame = 0;
    animTimer = 0.0f;
    animInterval = 0.1f;
    attackTimer = 0.0f;
    attackInterval = 0.65f;
    attackDamage = 60;

    // 10% chance to play groan sound
    int probability = rand() % 10;
    if (!probability) {
        sounds();
    }
}

// ---------- ConeheadZombie update ----------
void ConeheadZombie::update(float delta) {
    if (!isAttacking) {
        worldX -= speed * delta;
    }
    const auto& frames = isAttacking ? attack_frames : s_frames;
    int total = frames.size();
    if (total > 0) {
        animTimer += delta;
        if (animTimer >= animInterval) {
            animTimer -= animInterval;
            currentFrame = (currentFrame + 1) % total;
        }
    }
}

// ---------- ConeheadZombie draw ----------
void ConeheadZombie::draw(int screenX, int screenY) {
    const std::vector<IMAGE>& frames = isAttacking ? attack_frames : s_frames;
    int total = (int)frames.size();
    if (total > 0 && currentFrame < total) {
        const IMAGE& img = frames[currentFrame];
        putimagePng(screenX, screenY, &img, 0, 0, img.getwidth(), img.getheight());
    }
}

// ---------- ConeheadZombie takeDamage ----------
void ConeheadZombie::takeDamage(int damage) {
    hp -= damage;
    if (hp < 0) hp = 0;
}

// ---------- ConeheadZombie isDead ----------
bool ConeheadZombie::isDead() const {
    return hp <= 0;
}

// ---------- ConeheadZombie drawDead ----------
void ConeheadZombie::drawDead() {
}

// ---------- ConeheadZombie sounds ----------
void ConeheadZombie::sounds() {
    int id = rand() % 6 + 1;
    std::string ID = std::to_string(id);
    std::string file = "groan" + ID + ".mp3";
    audio.playEffect(file);
}