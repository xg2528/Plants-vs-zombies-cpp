#pragma once
#include <vector>
#include <easyx.h>

// 僵尸基类
class Zombies {
public:
    virtual ~Zombies() = default;
    virtual void update(float delta) = 0;
    virtual void draw(int screenX, int screenY) = 0;
    virtual void takeDamage(int damage) = 0;
    virtual bool isDead() const = 0;
    virtual void drawDead()=0;

    int row;               // 所在行（0-4）
    float worldX, worldY;  // 世界坐标（像素）
    bool isAttacking;      // 是否正在攻击植物
    int hp;                // 生命值
    // 实例动画状态
    int currentFrame;                  // 当前帧索引
    float animTimer;                   // 动画计时器
    float animInterval;                // 每帧间隔（秒）
    float attackTimer;       // 攻击冷却剩余时间（秒）
    float attackInterval;    // 攻击间隔（秒）
    int attackDamage;        // 每次攻击造成的伤害
    Zombies()
        : row(0), worldX(0.0f), worldY(0.0f), isAttacking(false), hp(0),
        currentFrame(0), animTimer(0.0f), animInterval(0.1f),
        attackTimer(0.0f), attackInterval(1.0f), attackDamage(10) {
    }
 
};

// 普通僵尸（所有实例共享同一套动画图片）
class NormalZombie : public Zombies {
public:
    // 构造函数：接收行、世界起始坐标（X, Y）
    NormalZombie(int row, float startX, float startY);
    void update(float delta) override;
    void draw(int screenX, int screenY) override;
    void takeDamage(int damage) override;
    bool isDead() const override;
    void drawDead()override;
    void sounds();

    // 静态方法：加载所有共享的动画图片（只需在游戏开始时调用一次）
    static void loadSharedImages();

private:
    float speed;                       // 向左移动速度（像素/秒）
    static const int WIDTH = 166;       // 矩形后备宽度
    static const int HEIGHT = 144;      // 矩形后备高度

    // 静态共享数据（所有僵尸共用）
    static std::vector<IMAGE> s_frames;   // 存储所有动画帧
    static std::vector<IMAGE> attack_frames;//储存攻击的动画帧
    static int s_totalFrames;             // 实际成功加载的帧数
    static int s_attackTotalFrames;
    static bool s_imagesLoaded;           // 是否已加载过图片
  
  
};