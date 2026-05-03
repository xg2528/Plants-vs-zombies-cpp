#pragma once
#include "GameState.h"
#include <easyx.h>
#include<memory>
#include"Game.h"
#include"Menu.h"
#include "Spawn.h"
#include<chrono>
#include"LevelData.h"
#include"LevelMeter.h"
#include "Bullet.h"   // 需要包含子弹基类头文件（如果尚未包含）
#include <algorithm>  // 用于 remove_if

class PlayingState : public GameState {
public:
    void onEnter() override;
    void onExit() override;
    void handleInput() override;
    void update() override;
    void render() override;

private:
    // 可以添加游戏需要的成员变量，比如背景图、植物列表等
    IMAGE background;
    IMAGE sunBack;
    float cameraX=0.0f;          // 当前水平偏移（像素）
    int scrollState=0;        // 0=初始停留, 1=向右滚动, 2=向右完成停留, 3=向左滚动, 4=滚动结束
    //std::chrono::steady_clock::time_point phaseStartTime;   // 状态开始时间（毫秒）
    int currentMusic=0;  // 0=无,1=lookatthesky,2=readysetplants,3=UraniwaNi
    struct Card {
        IMAGE img;          // 卡片图片
        IMAGE plantImg;     // 植物静态图片（用于跟随鼠标和放置后显示）
        int x=0, y=0;           // 卡片在窗口上的位置
        int cost=0;           // 阳光消耗
        bool isSelected=false;    // 是否被选中
        bool isCooling=false;        // 是否正在冷却
        float coolingRemain=0.0f;   // 剩余冷却时间（秒）
        // 总冷却时间（例如 7.5 秒）
        float totalCoolTime=7500.0f;
       int plantType=0;      // 植物类型（0=豌豆射手，1=向日葵等）
    };
    std::vector<Card> cards;  // 卡片列表
    int currentSun=150;            // 当前阳光数量
    Spawn spawn;  // 出怪管理器
    LevelData levelData;
    std::vector<std::unique_ptr<Bullet>> bullets;  // 子弹列表（放在其他容器附近）
	// 预览僵尸：开场镜头滚动时在右侧展示
	struct PreviewZombie {
    public:
	    int imgIndex;  // index into previewImages
		float worldX;
		float worldY;
	};
	std::vector<IMAGE> previewImages;  // owns loaded preview images
	std::vector<PreviewZombie> previewZombies;

	// 锲子
	IMAGE shovelImg;
	IMAGE shovelBackImg;
	bool shovelSelected = false;
	int shovelX = 500;
	int shovelY = 10;
	LevelMeter levelMeter;
};