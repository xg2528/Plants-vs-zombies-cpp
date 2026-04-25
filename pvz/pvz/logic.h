#pragma once
#include <vector>
#include <easyx.h>

// 阳光类：负责动画帧管理和绘制
class Sun {
public:
    int worldX, worldY;     // 世界坐标（相对于背景图片）
    int currentFrame;       // 当前动画帧索引
    float animTimer;        // 动画计时器（秒）

    Sun();                  // 随机生成位置并加载动画帧（共享）
    void update(float delta);   // 更新动画（每帧调用）
    void draw(int cameraX) const; // 绘制（传入摄像机偏移）
};