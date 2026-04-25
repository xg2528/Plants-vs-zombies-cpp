#pragma once
#include <easyx.h>

// 子弹基类（抽象类）
class Bullet {
public:
    // 构造函数：初始化位置、行、伤害等
    Bullet(float worldX, float worldY, int row, int damage, float speed);
    virtual ~Bullet() = default;

    // 纯虚函数：加载子弹图片（派生类必须实现）
    virtual void loadImg() = 0;

    // 更新逻辑：移动，返回是否还在激活状态（超出边界则失活）
    virtual bool update(float delta);

    // 绘制子弹（根据摄像机偏移计算屏幕坐标）
    virtual void draw(int cameraX) const;

    // 获取属性
    float getWorldX() const { return worldX; }
    int getRow() const { return row; }
    int getDamage() const { return damage; }
    bool isActive() const { return active; }
    void deactivate() { active = false; }
    bool canHit;          // 是否可以参与碰撞
protected:
    float worldX, worldY;   // 世界坐标（像素）
    int row;                // 所在行（0-4）
    int damage;             // 伤害值
    float speed;            // 移动速度（像素/秒，正数向右）
    bool active;            // 是否激活（超出边界则设置为 false）
    float lifeTimer;      // 存活时间（秒）
    IMAGE img;              // 子弹图片（派生类负责加载）
};

class PeaBullet : public Bullet {
public:
    PeaBullet(float x, float y, int r, int d);
    void loadImg() override;
       
};