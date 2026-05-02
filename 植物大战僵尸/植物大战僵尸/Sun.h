#pragma once
#include <vector>
#include <easyx.h>

class Sun {
public:
    float worldX, worldY;
    int currentFrame;
    float animTimer;
	bool isSunFlower; // 新增：是否是向日葵产生的阳光
	float speed; // 新增：阳光下降速度（像素/秒）
	int waitTime; // 新增：等待时间（秒），时间到了自动删除
    float waitTimer;
	int drop_time; // 新增：阳光落地时间（秒）
	float dropTimer; // 新增：落地计时器
	bool isCollected = false; // 新增：是否已被收集
    Sun();
    void update(float delta);
    void draw(int cameraX) const;
    IMAGE getFirstFrame() const {
        const auto& frames = getFrames();
        if (frames.empty()) return IMAGE(); // 返回默认空图
        return frames[0];
	}
    Sun(float worldX, float worldY, bool isSunFlower = false);
private:
    static const std::vector<IMAGE>& getFrames();
    
};